"""Upload a Zigbee OTA image to the gateway-module over UART.

Command line: `python ota_upload.py` reads port and image settings from
config.ini. The GUI (ota_gui.py) uses OtaUploader directly.
"""

import math
import queue
import struct
import sys
import threading
import time
import zlib

from serialport import MySerial

HEADER = [0xFF, 0xFC, 0xFC, 0xFF]

CMD_OTA_START         = 0xF0000000
CMD_OTA_BLOCK         = 0xF0000001
CMD_OTA_INSERT        = 0xF0000003
CMD_OTA_CANDIDATE_SET = 0xF0000005

# response cmd_id = request cmd_id | 0x8000, per zigbee_cmd_ota.h
RSP_OTA_START         = CMD_OTA_START | 0x8000
RSP_OTA_BLOCK         = CMD_OTA_BLOCK | 0x8000
RSP_OTA_UPLOAD_END    = 0xF0008002
RSP_OTA_INSERT        = CMD_OTA_INSERT | 0x8000
RSP_OTA_CANDIDATE_SET = CMD_OTA_CANDIDATE_SET | 0x8000

STATUS_FAIL = 0xFFFFFFFF
STATUS_BUSY = 0xFFFFFFFE  # a Zigbee device is downloading the inserted image

BLOCK_SIZE = 200          # frame length byte is 8 bits: 7 + 22 + 200 <= 255
MAX_RETRIES = 5
ACK_TIMEOUT = 1.0         # seconds
START_TIMEOUT = 30.0      # firmware erases the whole FOTA bank (0x74000) before answering
END_TIMEOUT = 30.0        # firmware reads back the whole image to verify the CRC32


class UploadCancelled(Exception):
    pass


def frame_checksum(command: list[int]):
    """8-bit checksum of the gateway frame: ~(sum of length byte + payload)."""
    return ~sum(command[4:]) & 0xFF


def build_frame(cmd_id: int, params: bytes = b""):
    # length covers CmdID(4) + Addr(2) + Mode(1) + params
    body = list(struct.pack("<IHB", cmd_id, 0x0000, 0x00)) + list(params)
    command = HEADER + [len(body)] + body
    command.append(frame_checksum(command))
    return command


def find_complete_frame(buf, start):
    """Index of the first header at or after start that holds a complete frame
    with a good checksum, or None."""
    for i in range(start, len(buf) - 4):
        if buf[i:i+4] != HEADER:
            continue
        end = i + 4 + 1 + buf[i+4] + 1
        if buf[i+4] >= 7 and end <= len(buf) and frame_checksum(buf[i:end-1]) == buf[end-1]:
            return i
    return None


def payload_to_uint32(payload):
    if len(payload) < 4:
        return None
    return payload[0] | (payload[1] << 8) | (payload[2] << 16) | (payload[3] << 24)


def status_ok(payload):
    # firmware returns 4 bytes 0x00000000 on success, 0xFFFFFFFF on failure
    return payload_to_uint32(payload) == 0


def candidate_set_ok(payload):
    return len(payload) >= 1 and payload[0] == 0


def block_ack(expect):
    """True: ack for this block, False: firmware rejected it, None: ack of another block."""
    def check(payload):
        value = payload_to_uint32(payload)
        if value == expect:
            return True
        if value == STATUS_FAIL:
            return False
        return None
    return check


class OtaUploader:
    """Runs the whole upload sequence: start, blocks, candidate set, insert.

    log(str) receives progress messages, progress(done, total) is called after
    every acknowledged block. Call cancel() from another thread to abort.
    """

    def __init__(self, port, baudrate, ota_image: bytes, image_type, manuf_code,
                 file_version, log=print, progress=None):
        self.port = port
        self.baudrate = baudrate
        self.ota_image = ota_image
        self.image_type = image_type
        self.manuf_code = manuf_code
        self.file_version = file_version
        self.log = log
        self.progress = progress
        self.total_block = max(1, math.ceil(len(ota_image) / BLOCK_SIZE))
        self.response_queue = queue.Queue()
        self.serial = None
        self._stop = threading.Event()

    def cancel(self):
        self._stop.set()

    # ---------------- receive side ----------------
    def _handle_hex_command(self, msg):
        if frame_checksum(msg[:-1]) != msg[-1]:
            self.log(f"[WARN] recv checksum error, drop frame: {msg}")
            return False

        cmd_id = struct.unpack("<I", bytes(msg[5:9]))[0]
        payload = msg[12:-1]
        self.response_queue.put((cmd_id, payload))
        return True

    def _read_serial_msg(self):
        buf = []   # accumulated UART bytes

        while not self._stop.is_set():
            try:
                incoming = list(self.serial.serPort.read(size=99))
            except Exception:
                return   # port closed or unplugged
            if not incoming:
                continue

            buf.extend(incoming)

            while True:
                # ---------- 1. find header ----------
                try:
                    header_index = buf.index(HEADER[0])
                except ValueError:
                    buf.clear()   # no 0xFF at all: everything is garbage
                    break

                del buf[:header_index]   # drop garbage before the candidate header
                if len(buf) < 5:
                    break  # wait for more data

                if buf[:4] != HEADER:
                    del buf[0]
                    continue

                # ---------- 2. length ----------
                length = buf[4]  # CmdID(4) + Addr(2) + Mode(1) + payload(n)
                total_msg_len = 4 + 1 + length + 1  # header + length + data + checksum
                if length < 7:
                    del buf[0]
                    continue

                # ---------- 3. wait for the whole frame ----------
                if len(buf) < total_msg_len:
                    # a corrupted length byte would stall a valid frame queued behind it
                    nxt = find_complete_frame(buf, 1)
                    if nxt is None:
                        break
                    del buf[:nxt]
                    continue

                # ---------- 4. take the frame ----------
                msg = buf[:total_msg_len]
                if self._handle_hex_command(msg):
                    del buf[:total_msg_len]
                else:
                    # length byte may be corrupted: skip this header only and resync,
                    # otherwise a valid frame behind it would be thrown away too
                    del buf[0]

    # ---------------- send side ----------------
    def _drain_responses(self):
        while True:
            try:
                self.response_queue.get_nowait()
            except queue.Empty:
                return

    def _send_with_retry(self, command, expected_cmd_id, validate_payload=None,
                         label="command", max_retries=MAX_RETRIES, timeout=ACK_TIMEOUT):
        """Send a frame and wait for its ACK.

        Responses with another cmd_id, or for another block (late ACKs of an earlier
        retry), are ignored and we keep waiting; resending on them would make the
        firmware see the same block twice. The frame is resent only on timeout or
        when the firmware reports a failure for this very frame.
        """
        for attempt in range(1, max_retries + 1):
            if self._stop.is_set():
                raise UploadCancelled()
            self._drain_responses()
            self.serial.send_hex_serial(command)

            deadline = time.monotonic() + timeout
            nacked = False
            while True:
                if self._stop.is_set():
                    raise UploadCancelled()
                remaining = deadline - time.monotonic()
                if remaining <= 0:
                    self.log(f"[{label}] no response within {timeout}s, retry {attempt}/{max_retries}")
                    break
                try:
                    # short poll so cancel() is honoured quickly
                    cmd_id, payload = self.response_queue.get(timeout=min(remaining, 0.2))
                except queue.Empty:
                    continue

                if cmd_id != expected_cmd_id:
                    self.log(f"[{label}] ignore unexpected cmd_id=0x{cmd_id:08X}")
                    continue

                if payload_to_uint32(payload) == STATUS_BUSY:
                    self.log(f"[{label}] gateway busy: a Zigbee OTA is in progress")
                    return False, payload

                if validate_payload is None:
                    return True, payload

                result = validate_payload(payload)
                if result is None:
                    self.log(f"[{label}] ignore stale ack, payload={payload}")
                    continue
                if result:
                    return True, payload

                self.log(f"[{label}] ack reported failure, payload={payload}, retry {attempt}/{max_retries}")
                nacked = True
                break

            if nacked:
                time.sleep(0.1)

        return False, None

    def _upload_end_ok(self, payload):
        if len(payload) == 0:
            # firmware without the end-to-end CRC check: nothing to validate
            self.log("[WARN] firmware did not report upload status, image is not CRC verified")
            return True
        return status_ok(payload)

    def _build_block(self, cur_block, data: bytes):
        params = struct.pack("<HHIIIIH", self.image_type, self.manuf_code, self.file_version,
                             len(self.ota_image), self.total_block, cur_block, len(data)) + data
        return build_frame(CMD_OTA_BLOCK, params)

    # ---------------- sequence ----------------
    def run(self):
        """Return True when the image was uploaded and set as candidate."""
        self._stop.clear()
        self.serial = MySerial(self.port, self.baudrate, log=self.log)
        if not self.serial.open():
            return False

        reader = threading.Thread(target=self._read_serial_msg, daemon=True)
        reader.start()
        try:
            return self._run_sequence()
        except UploadCancelled:
            self.log("Upload cancelled by user")
            return False
        except Exception as e:
            self.log(f"Upload error: {e}")
            return False
        finally:
            self._stop.set()
            reader.join(timeout=1)
            self.serial.close()

    def _run_sequence(self):
        image_crc32 = zlib.crc32(self.ota_image) & 0xFFFFFFFF
        total_block = self.total_block

        self.log("Start upload OTA image")
        self.log(f"Image Size: {len(self.ota_image)}")
        self.log(f"Image CRC32: 0x{image_crc32:08X}")
        self.log(f"Image type: 0x{self.image_type:04X}")
        self.log(f"Manufacture Code: 0x{self.manuf_code:04X}")
        self.log(f"File Version: 0x{self.file_version:08X}")

        ## ota start, with the CRC32 of the whole file so the firmware can verify the upload
        self.log("OTA Start (gateway erases the FOTA bank, may take a while)...")
        ok, _ = self._send_with_retry(build_frame(CMD_OTA_START, struct.pack("<I", image_crc32)),
                                      RSP_OTA_START, validate_payload=status_ok,
                                      label="OTA Start", timeout=START_TIMEOUT)
        if not ok:
            self.log("OTA Start failed, abort upload")
            return False

        ## upload ota image
        for i in range(total_block):
            command = self._build_block(i, self.ota_image[i*BLOCK_SIZE:(i+1)*BLOCK_SIZE])

            if i == total_block - 1:
                # only retry on timeout: a reported failure means the image did not verify
                ok, payload = self._send_with_retry(command, RSP_OTA_UPLOAD_END,
                                                    label=f"Block {i}/{total_block-1} (last)",
                                                    timeout=END_TIMEOUT)
                if ok and not self._upload_end_ok(payload):
                    self.log("Gateway rejected the uploaded image (size/CRC32/header check failed), abort upload")
                    return False
            else:
                ok, _ = self._send_with_retry(command, RSP_OTA_BLOCK, validate_payload=block_ack(i),
                                              label=f"Block {i}/{total_block-1}")

            if not ok:
                self.log(f"Block {i} failed, abort upload")
                return False
            if self.progress:
                self.progress(i + 1, total_block)

        ## set candidate before insert: insert broadcasts Image Notify right away,
        ## and a Query Next Image arriving before the candidate is set gets NO_IMAGE_AVAILABLE
        ok, _ = self._send_with_retry(build_frame(CMD_OTA_CANDIDATE_SET, struct.pack("<H", 0xFFFF)),
                                      RSP_OTA_CANDIDATE_SET, validate_payload=candidate_set_ok,
                                      label="Candidate Set", timeout=30)
        if not ok:
            self.log("Candidate Set failed after retries, abort upload")
            return False

        ## insert ota image
        ok, _ = self._send_with_retry(build_frame(CMD_OTA_INSERT), RSP_OTA_INSERT,
                                      validate_payload=status_ok, label="Insert", timeout=30)
        if not ok:
            self.log("Insert OTA file failed, abort upload")
            return False

        self.log("Finished Upload OTA Image")
        return True


def main():
    from pathlib import Path
    from tqdm import tqdm
    import config

    ota_image = Path(config.ota_filename).read_bytes()
    bar = tqdm(total=max(1, math.ceil(len(ota_image) / BLOCK_SIZE)))

    def progress(done, total):
        bar.update(done - bar.n)

    uploader = OtaUploader(config.port, config.baudrate, ota_image,
                           config.ota_image_type, config.ota_manuf_code,
                           config.ota_file_version, log=tqdm.write, progress=progress)
    ok = uploader.run()
    bar.close()
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
