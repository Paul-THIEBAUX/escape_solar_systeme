import struct, zlib, sys

path = sys.argv[1]
with open(path, "rb") as f:
    data = f.read()

assert data[:8] == b'\x89PNG\r\n\x1a\n', "Signature PNG invalide"
pos = 8
while pos < len(data):
    length = struct.unpack(">I", data[pos:pos+4])[0]
    ctype = data[pos+4:pos+8]
    chunk_data = data[pos+8:pos+8+length]
    crc_stored = struct.unpack(">I", data[pos+8+length:pos+12+length])[0]
    crc_calc = zlib.crc32(ctype + chunk_data) & 0xffffffff
    status = "OK" if crc_calc == crc_stored else "CRC INVALIDE !!"
    print(f"{ctype} - length={length} - {status} (stored={crc_stored:08x}, calc={crc_calc:08x})")
    pos += 12 + length
    if ctype == b'IEND':
        break