"""Create a Zigbee OTA Upgrade image file from a firmware binary.

Based on create_image.exe
(components/network/zigbee/zboss/devtools/OTA/create_image/create_image.c).
The firmware is always LZMA compressed and can optionally be signed
(ECDSA P-256), and a 16-byte Rafael image header telling the device whether
the image is compressed/signed is placed in front of it (see rafael_image.py).

Usage:
    python create_image.py [-pk private.pem [-pub public.pem]] {src_file} {dest_file} {file_version} {manufacturer_code} [{image_type}]
    python create_image.py ota_client.bin ota_client.ota v01.02.01.01 0xdead 0x0012
    python create_image.py -pk private.pem -pub public.pem ota_client.bin ota_client.ota v01.02.01.01 0xdead 0x0012

OTA file layout (Zigbee 095264r23, 6.3 OTA File Format), little endian:
    OTA header (56 bytes)
    upgrade image sub-element: tag 0x0000 (2) | length (4) | Rafael header (16) | payload
    hash sub-element:          tag 0x0003 (2) | length 16 (4) | AES-MMO hash (16)
payload is LZMA(firmware), or sign(LZMA(sign(firmware))) when signed.
The hash covers everything from the first header byte up to the end of the
upgrade image sub-element (6.3.10.1). The Rafael header therefore starts at
file offset 62, where the device starts storing the image.
"""

import argparse
import re
import struct
import sys

import rafael_image

OTA_FILE_ID = 0x0BEEF11E
OTA_HEADER_VERSION = 0x0100
OTA_HEADER_LENGTH = 0x0038
OTA_HEADER_FC = 0x0010
OTA_DEFAULT_IMAGE_TYPE = 0x0141
OTA_STACK_VERSION_PRO = 0x0002
OTA_HEADER_STRING = b"The latest and greatest upgrade."  # exactly 32 bytes

OTA_FW_TAG_ID = 0x0000
OTA_HASH_TAG_ID = 0x0003
OTA_HASH_LENGTH = 16

# <I file_id, H header_version, H header_length, H fc, H manufacturer_code,
#  H image_type, I file_version, H stack_version, 32s header_string,
#  I total_image_size>
OTA_HEADER_FMT = "<IHHHHHIH32sI"
SUB_ELEMENT_HDR_FMT = "<HI"
OTA_IMAGE_DATA_OFFSET = (struct.calcsize(OTA_HEADER_FMT) +
                         struct.calcsize(SUB_ELEMENT_HDR_FMT))  # 62


# ---------------------------------------------------------------------------
# AES-128 block encryption (pure Python, encrypt only)
# ---------------------------------------------------------------------------
def _build_tables():
    sbox = [0] * 256
    p = q = 1
    while True:
        # multiply p by 3, divide q by 3 in GF(2^8)
        p = p ^ ((p << 1) & 0xFF) ^ (0x1B if p & 0x80 else 0)
        q ^= q << 1
        q ^= q << 2
        q ^= q << 4
        q &= 0xFF
        if q & 0x80:
            q ^= 0x09
        x = q ^ (q << 1 | q >> 7) ^ (q << 2 | q >> 6) ^ \
            (q << 3 | q >> 5) ^ (q << 4 | q >> 4)
        sbox[p] = (x ^ 0x63) & 0xFF
        if p == 1:
            break
    sbox[0] = 0x63

    def xtime(b):
        return ((b << 1) ^ (0x1B if b & 0x80 else 0)) & 0xFF

    # T-table for column (s, s, s*3, s*2) layout; rotated copies for rows
    te0 = [0] * 256
    for i in range(256):
        s = sbox[i]
        s2 = xtime(s)
        s3 = s2 ^ s
        te0[i] = (s2 << 24) | (s << 16) | (s << 8) | s3
    te1 = [((t >> 8) | (t << 24)) & 0xFFFFFFFF for t in te0]
    te2 = [((t >> 16) | (t << 16)) & 0xFFFFFFFF for t in te0]
    te3 = [((t >> 24) | (t << 8)) & 0xFFFFFFFF for t in te0]
    return sbox, te0, te1, te2, te3


_SBOX, _TE0, _TE1, _TE2, _TE3 = _build_tables()
_RCON = (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36)


def _expand_key(key):
    w = list(struct.unpack(">4I", key))
    sb = _SBOX
    for i in range(4, 44):
        t = w[i - 1]
        if i % 4 == 0:
            t = ((sb[(t >> 16) & 0xFF] << 24) | (sb[(t >> 8) & 0xFF] << 16) |
                 (sb[t & 0xFF] << 8) | sb[t >> 24]) ^ (_RCON[i // 4 - 1] << 24)
        w.append(w[i - 4] ^ t)
    return w


def aes128_encrypt_block(key, block):
    """Encrypt one 16-byte block with a 16-byte key."""
    rk = _expand_key(key)
    te0, te1, te2, te3, sb = _TE0, _TE1, _TE2, _TE3, _SBOX
    s0, s1, s2, s3 = struct.unpack(">4I", block)
    s0 ^= rk[0]
    s1 ^= rk[1]
    s2 ^= rk[2]
    s3 ^= rk[3]
    for r in range(1, 10):
        k = 4 * r
        t0 = (te0[s0 >> 24] ^ te1[(s1 >> 16) & 0xFF] ^
              te2[(s2 >> 8) & 0xFF] ^ te3[s3 & 0xFF] ^ rk[k])
        t1 = (te0[s1 >> 24] ^ te1[(s2 >> 16) & 0xFF] ^
              te2[(s3 >> 8) & 0xFF] ^ te3[s0 & 0xFF] ^ rk[k + 1])
        t2 = (te0[s2 >> 24] ^ te1[(s3 >> 16) & 0xFF] ^
              te2[(s0 >> 8) & 0xFF] ^ te3[s1 & 0xFF] ^ rk[k + 2])
        t3 = (te0[s3 >> 24] ^ te1[(s0 >> 16) & 0xFF] ^
              te2[(s1 >> 8) & 0xFF] ^ te3[s2 & 0xFF] ^ rk[k + 3])
        s0, s1, s2, s3 = t0, t1, t2, t3
    out = []
    for a, b, c, d, k in ((s0, s1, s2, s3, rk[40]), (s1, s2, s3, s0, rk[41]),
                          (s2, s3, s0, s1, rk[42]), (s3, s0, s1, s2, rk[43])):
        out.append(((sb[a >> 24] << 24) | (sb[(b >> 16) & 0xFF] << 16) |
                    (sb[(c >> 8) & 0xFF] << 8) | sb[d & 0xFF]) ^ k)
    return struct.pack(">4I", *out)


# ---------------------------------------------------------------------------
# AES-MMO hash (Zigbee spec B.6, Matyas-Meyer-Oseas)
# ---------------------------------------------------------------------------
def aes_mmo_hash(message):
    """Return the 16-byte AES-MMO hash of message (length < 2^29 bytes)."""
    bit_len = len(message) * 8
    if bit_len >= 1 << 32:
        raise ValueError("message too long for AES-MMO")

    # Padding: M || 0x80 || 0x00... || length, block aligned.
    #   < 2^16 bits: 16-bit big-endian bit length in the last 2 bytes
    #   otherwise  : 32-bit big-endian bit length followed by 2 zero bytes
    if bit_len < 0x10000:
        tail = struct.pack(">H", bit_len)
    else:
        tail = struct.pack(">IH", bit_len, 0)
    pad_len = (-(len(message) + 1 + len(tail))) % 16
    padded = bytes(message) + b"\x80" + b"\x00" * pad_len + tail

    h = bytes(16)
    for off in range(0, len(padded), 16):
        block = padded[off:off + 16]
        c = aes128_encrypt_block(h, block)
        h = bytes(x ^ y for x, y in zip(c, block))
    return h


# ---------------------------------------------------------------------------
# OTA image
# ---------------------------------------------------------------------------
def wrap_ota_image(image_data, file_version, manufacturer_code,
                   image_type=OTA_DEFAULT_IMAGE_TYPE):
    """Wrap image_data as-is into a Zigbee OTA upgrade file (create_image.exe
    format) and return it."""
    header_size = struct.calcsize(OTA_HEADER_FMT)
    sub_hdr_size = struct.calcsize(SUB_ELEMENT_HDR_FMT)
    total_size = (header_size + sub_hdr_size + len(image_data) +
                  sub_hdr_size + OTA_HASH_LENGTH)

    header = struct.pack(OTA_HEADER_FMT, OTA_FILE_ID, OTA_HEADER_VERSION,
                         OTA_HEADER_LENGTH, OTA_HEADER_FC, manufacturer_code,
                         image_type, file_version, OTA_STACK_VERSION_PRO,
                         OTA_HEADER_STRING, total_size)
    body = (header +
            struct.pack(SUB_ELEMENT_HDR_FMT, OTA_FW_TAG_ID, len(image_data)) +
            bytes(image_data))
    digest = aes_mmo_hash(body)
    return (body +
            struct.pack(SUB_ELEMENT_HDR_FMT, OTA_HASH_TAG_ID, OTA_HASH_LENGTH) +
            digest)


def create_ota_image(firmware, file_version, manufacturer_code,
                     image_type=OTA_DEFAULT_IMAGE_TYPE, private_key=None,
                     public_key=None, log=None):
    """Compress (and sign, when private_key is given) firmware, put the Rafael
    header in front and wrap it into a Zigbee OTA upgrade file."""
    payload = rafael_image.build_payload(firmware, private_key, public_key, log)
    return wrap_ota_image(payload, file_version, manufacturer_code, image_type)


def parse_rafael_info(image):
    """Return the Rafael header dict of an OTA file, or None if it has none."""
    if len(image) < OTA_IMAGE_DATA_OFFSET:
        return None
    tag, length = struct.unpack_from(SUB_ELEMENT_HDR_FMT, image,
                                     struct.calcsize(OTA_HEADER_FMT))
    if tag != OTA_FW_TAG_ID:
        return None
    info = rafael_image.parse_rafael_header(image[OTA_IMAGE_DATA_OFFSET:])
    if info is None:
        return None
    expected = length - rafael_image.RAFAEL_HEADER_SIZE
    if info["image_len"] != expected:
        raise ValueError("Rafael header image length %d does not match the "
                         "upgrade image sub-element (%d)"
                         % (info["image_len"], expected))
    return info


def parse_ota_header(image):
    """Return (image_type, manufacturer_code, file_version) of an OTA file."""
    header_size = struct.calcsize(OTA_HEADER_FMT)
    if len(image) < header_size:
        raise ValueError("file too short to be an OTA image")
    (file_id, _hdr_ver, _hdr_len, _fc, manufacturer_code, image_type,
     file_version, _stack, _string, total_size) = struct.unpack_from(
        OTA_HEADER_FMT, image)
    if file_id != OTA_FILE_ID:
        raise ValueError("not an OTA image (file id 0x%08X)" % file_id)
    if total_size != len(image):
        raise ValueError("OTA header size %d does not match file size %d"
                         % (total_size, len(image)))
    return image_type, manufacturer_code, file_version


def format_version(value):
    """uint32 to the 'vAA.BB.CC.DD' form accepted by parse_version()."""
    return "v%02X.%02X.%02X.%02X" % ((value >> 24) & 0xFF, (value >> 16) & 0xFF,
                                     (value >> 8) & 0xFF, value & 0xFF)


def parse_version(text):
    """Parse 'vAA.BB.CC.DD' (hex fields, 1-2 digits each) to a uint32."""
    m = re.fullmatch(r"v([0-9A-Fa-f]{1,2})\.([0-9A-Fa-f]{1,2})\."
                     r"([0-9A-Fa-f]{1,2})\.([0-9A-Fa-f]{1,2})", text)
    if not m:
        raise ValueError("incorrect version format : use format vAA.BB.CC.DD "
                         "i.s.o. %s" % text)
    a, b, c, d = (int(g, 16) for g in m.groups())
    return (a << 24) | (b << 16) | (c << 8) | d


def parse_hex16(text, what):
    """Parse a '0x01AB' style 16-bit value (exactly 4 hex digits)."""
    if not re.fullmatch(r"0[xX][0-9A-Fa-f]{4}", text):
        raise ValueError("Invalid %s : use format 0x01AB instead of %s"
                         % (what, text))
    return int(text, 16)


def main(argv):
    parser = argparse.ArgumentParser(
        description="Create a compressed (optionally signed) Zigbee OTA image")
    parser.add_argument("-pk", "--private-key",
                        help="ECDSA P-256 private key (PEM); when given, the "
                             "image is signed, compressed and signed again")
    parser.add_argument("-pub", "--public-key",
                        help="ECDSA P-256 public key (PEM) used to verify the "
                             "signatures right after signing")
    parser.add_argument("src_file")
    parser.add_argument("dest_file")
    parser.add_argument("file_version", help="vAA.BB.CC.DD")
    parser.add_argument("manufacturer_code", help="0x01AB")
    parser.add_argument("image_type", nargs="?", help="0x01AB (default 0x0141)")
    args = parser.parse_args(argv[1:])

    try:
        file_version = parse_version(args.file_version)
        manufacturer_code = parse_hex16(args.manufacturer_code, "manufacturer code")
        image_type = (parse_hex16(args.image_type, "image type")
                      if args.image_type else OTA_DEFAULT_IMAGE_TYPE)
    except ValueError as e:
        print(e)
        return 1

    if args.public_key and not args.private_key:
        print("Public key is given without private key")
        return 1

    try:
        with open(args.src_file, "rb") as f:
            firmware = f.read()
    except OSError:
        print("please specify input FW image")
        return 1

    print("setting version nbr = 0x%08X" % file_version)
    print("setting manufacturer code = 0x%04X" % manufacturer_code)
    print("setting image_type = 0x%04X" % image_type)
    print("signing: %s" % ("enabled" if args.private_key else "disabled"))

    try:
        private_key = (rafael_image.load_private_key(args.private_key)
                       if args.private_key else None)
        public_key = (rafael_image.load_public_key(args.public_key)
                      if args.public_key else None)
        image = create_ota_image(firmware, file_version, manufacturer_code,
                                 image_type, private_key, public_key, log=print)
    except rafael_image.RafaelImageError as e:
        print("error: %s" % e)
        return 1
    with open(args.dest_file, "wb") as f:
        f.write(image)

    print("\nImage is written to file %s" % args.dest_file)
    print("\nAES MMO hash:")
    print(" " + " ".join("%x" % b for b in image[-OTA_HASH_LENGTH:]))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
