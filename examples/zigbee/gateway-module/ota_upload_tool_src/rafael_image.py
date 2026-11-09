"""Compression, signing and the Rafael image header for RT58x OTA payloads.

Ported from rafael_ota_tool.py / rafael_sign_image.py of the Rafael Matter
port (examples/zigbee/gateway-module/ref). Errors raise RafaelImageError
instead of exiting, so the GUI can report them.

Payload produced by build_payload():
    unsigned: LZMA(firmware)
    signed:   sign(LZMA(sign(firmware)))   ECDSA P-256 / SHA-256

The Rafael image header placed in front of the payload tells the device how to
hand the image to the bootloader (all fields little endian):
    magic "RFOT" (u32) | version (u8) | header size (u8) | image info (u8) |
    reserved (u8) | signature length (u32) | image length (u32)
Image info and signature length map to fota_information_t of the bootloader.
"""

import lzma
import struct

try:
    from cryptography.exceptions import InvalidSignature
    from cryptography.hazmat.primitives import hashes, serialization
    from cryptography.hazmat.primitives.asymmetric import ec
    from cryptography.hazmat.primitives.asymmetric.utils import decode_dss_signature
except ImportError:
    ec = None

RAFAEL_HEADER_MAGIC = 0x544F4652  # "RFOT"
RAFAEL_HEADER_VERSION = 1
RAFAEL_HEADER_FORMAT = "<IBBBBII"
RAFAEL_HEADER_SIZE = struct.calcsize(RAFAEL_HEADER_FORMAT)
FOTA_IMAGE_INFO_COMPRESSED = 0x01
FOTA_IMAGE_INFO_SIGNATURE = 0x02

SBIN_MAGIC = 0x58BF4E53
SBIN_ALIGNMENT = 256
SBIN_TRAILER_SIZE = 256
ECDSA_P256_COORD_SIZE = 32
IMAGE_LENGTH_OFFSET = 0x28

MAX_INPUT_IMAGE_SIZE = 1984 * 1024


class RafaelImageError(Exception):
    pass


# ---------------------------------------------------------------------------
# keys
# ---------------------------------------------------------------------------
def signing_available():
    """Whether the "cryptography" package needed for signing is installed."""
    return ec is not None


def _require_crypto():
    if not signing_available():
        raise RafaelImageError('Signing requires the "cryptography" Python package '
                               "(pip install cryptography)")


def load_private_key(path):
    _require_crypto()
    try:
        with open(path, "rb") as f:
            key = serialization.load_pem_private_key(f.read(), password=None)
    except OSError as e:
        raise RafaelImageError(f"Cannot read private key {path}: {e.strerror}")
    except (ValueError, TypeError):
        raise RafaelImageError(f"{path} is not an unencrypted PEM private key")
    if not isinstance(key, ec.EllipticCurvePrivateKey) or not isinstance(key.curve, ec.SECP256R1):
        raise RafaelImageError(f"{path} is not an ECDSA P-256 (prime256v1) private key")
    return key


def load_public_key(path):
    _require_crypto()
    try:
        with open(path, "rb") as f:
            key = serialization.load_pem_public_key(f.read())
    except OSError as e:
        raise RafaelImageError(f"Cannot read public key {path}: {e.strerror}")
    except ValueError:
        raise RafaelImageError(f"{path} is not a PEM public key")
    if not isinstance(key, ec.EllipticCurvePublicKey) or not isinstance(key.curve, ec.SECP256R1):
        raise RafaelImageError(f"{path} is not an ECDSA P-256 (prime256v1) public key")
    return key


# ---------------------------------------------------------------------------
# signing
# ---------------------------------------------------------------------------
def patch_image_length(data, log=None):
    """Store the image length at IMAGE_LENGTH_OFFSET, checked by the bootloader
    before signature verification."""
    if len(data) < IMAGE_LENGTH_OFFSET + 4:
        raise RafaelImageError("Input image is too small to hold the image length field")
    current = struct.unpack_from("<I", data, IMAGE_LENGTH_OFFSET)[0]
    if current not in (0, 0xFFFFFFFF, len(data)) and log:
        log(f"[WARN] Overwriting non-empty value 0x{current:08X} at image length "
            f"offset 0x{IMAGE_LENGTH_OFFSET:X}")
    patched = bytearray(data)
    struct.pack_into("<I", patched, IMAGE_LENGTH_OFFSET, len(data))
    return bytes(patched)


def sbin_signature_location(data_len):
    return (data_len + SBIN_ALIGNMENT - 1) & ~(SBIN_ALIGNMENT - 1)


def sbin_size(data_len):
    """Size of data_len bytes after the signature trailer is appended."""
    return sbin_signature_location(data_len) + SBIN_TRAILER_SIZE


def sbin_append_signature(data, r, s):
    sig_location = sbin_signature_location(len(data))
    trailer = struct.pack("<II", SBIN_MAGIC, len(data))
    trailer += r.to_bytes(ECDSA_P256_COORD_SIZE, "little")
    trailer += s.to_bytes(ECDSA_P256_COORD_SIZE, "little")
    trailer += b"\xff" * (SBIN_TRAILER_SIZE - len(trailer))
    return data + b"\xff" * (sig_location - len(data)) + trailer


def sign_image(data, private_key, public_key, patch_length, log=None):
    """Sign data with ECDSA P-256/SHA-256 and append the signature trailer.
    The signature is verified with public_key when one is given."""
    if patch_length:
        data = patch_image_length(data, log)
    signature = private_key.sign(data, ec.ECDSA(hashes.SHA256()))
    if public_key is not None:
        try:
            public_key.verify(signature, data, ec.ECDSA(hashes.SHA256()))
        except InvalidSignature:
            raise RafaelImageError("Signature verification failed: public key does not "
                                   "match private key")
    r, s = decode_dss_signature(signature)
    return sbin_append_signature(data, r, s)


# ---------------------------------------------------------------------------
# compression
# ---------------------------------------------------------------------------
def lzma_compress(data):
    """Compress into the .lzma (ALONE) format expected by the RT58x bootloader."""
    # Match pylzma defaults: BT4, fastBytes=128, no EOS marker, dict=2^16, lc=3, lp=0, pb=2
    filters = [{"id": lzma.FILTER_LZMA1, "dict_size": 1 << 16,
                "lc": 3, "lp": 0, "pb": 2,
                "mode": lzma.MODE_NORMAL, "nice_len": 128,
                "mf": lzma.MF_BT4, "depth": 0}]
    # FORMAT_RAW: pure LZMA1 stream without header or EOS marker
    comp = lzma.LZMACompressor(format=lzma.FORMAT_RAW, filters=filters)
    stream = comp.compress(data) + comp.flush()
    props = lzma.compress(b"", format=lzma.FORMAT_ALONE, filters=filters)[:5]
    # .lzma ALONE layout: 5-byte props | 8-byte LE uncompressed size | stream
    return props + struct.pack("<Q", len(data)) + stream


# ---------------------------------------------------------------------------
# Rafael header + payload
# ---------------------------------------------------------------------------
def generate_rafael_header(image_info, signature_len, image_len):
    return struct.pack(RAFAEL_HEADER_FORMAT, RAFAEL_HEADER_MAGIC, RAFAEL_HEADER_VERSION,
                       RAFAEL_HEADER_SIZE, image_info, 0, signature_len, image_len)


def parse_rafael_header(data):
    """Return dict of the Rafael header at the start of data, or None if absent."""
    if len(data) < RAFAEL_HEADER_SIZE:
        return None
    (magic, version, size, image_info, _reserved,
     signature_len, image_len) = struct.unpack_from(RAFAEL_HEADER_FORMAT, data)
    if magic != RAFAEL_HEADER_MAGIC:
        return None
    return {"version": version, "header_size": size, "image_info": image_info,
            "compressed": bool(image_info & FOTA_IMAGE_INFO_COMPRESSED),
            "signed": bool(image_info & FOTA_IMAGE_INFO_SIGNATURE),
            "signature_len": signature_len, "image_len": image_len}


def sign_firmware(firmware, private_key, public_key=None, log=None):
    """Return firmware signed only (no compression, no Rafael header), the same
    as the first signing step of build_payload()."""
    log = log or (lambda s: None)
    size = sbin_size(len(firmware))
    if size > MAX_INPUT_IMAGE_SIZE:
        raise RafaelImageError(f"Firmware is {size} bytes after signing, "
                               f"larger than the {MAX_INPUT_IMAGE_SIZE // 1024} KB limit")
    signed = sign_image(firmware, private_key, public_key, patch_length=True, log=log)
    log(f"  signed: {len(firmware)} -> {len(signed)} bytes")
    log(f"  signature verified with public key: {'yes' if public_key is not None else 'no'}")
    return signed


def build_payload(firmware, private_key=None, public_key=None, log=None):
    """Return Rafael header + payload for firmware bytes.

    Compression is always applied. With private_key the image is signed,
    compressed and signed again.
    """
    log = log or (lambda s: None)
    size = sbin_size(len(firmware)) if private_key is not None else len(firmware)
    if size > MAX_INPUT_IMAGE_SIZE:
        raise RafaelImageError(f"Firmware is {size} bytes{' after signing' if private_key else ''}, "
                               f"larger than the {MAX_INPUT_IMAGE_SIZE // 1024} KB limit")

    if private_key is None:
        payload = lzma_compress(firmware)
        log(f"  compressed: {len(firmware)} -> {len(payload)} bytes "
            f"({len(payload) * 100 / len(firmware):.1f}%)")
        header = generate_rafael_header(FOTA_IMAGE_INFO_COMPRESSED, 0, len(payload))
        return header + payload

    signed = sign_image(firmware, private_key, public_key, patch_length=True, log=log)
    compressed = lzma_compress(signed)
    payload = sign_image(compressed, private_key, public_key, patch_length=False, log=log)
    log(f"  signed: {len(firmware)} -> {len(signed)} bytes")
    log(f"  compressed: {len(signed)} -> {len(compressed)} bytes "
        f"({len(compressed) * 100 / len(signed):.1f}%)")
    log(f"  signed again: {len(payload)} bytes (signature length {len(compressed)})")
    log(f"  signature verified with public key: {'yes' if public_key is not None else 'no'}")
    header = generate_rafael_header(FOTA_IMAGE_INFO_COMPRESSED | FOTA_IMAGE_INFO_SIGNATURE,
                                    len(compressed), len(payload))
    return header + payload
