"""Build item expansion fixtures in an explicit output directory (never deploys)."""
import argparse
import struct
from pathlib import Path


def make_fixture(kernel_path: Path, mitem_path: Path, price_path: Path, output: Path):
    data = kernel_path.read_bytes()
    count = struct.unpack_from('<I', data)[0]
    if count != 56:
        raise ValueError('Expected a 56-section kernel')
    offsets = list(struct.unpack_from('<56I', data, 4))
    if offsets[0] < 228 or offsets != sorted(offsets) or offsets[-1] > len(data):
        raise ValueError('Invalid kernel offsets')
    sections = [data[a:b] for a, b in zip(offsets, offsets[1:] + [len(data)])]
    if len(sections[7]) != 33 * 24 or len(sections[8]) != 166 * 4:
        raise ValueError('Fixture input must have vanilla item sections')
    text = bytearray(sections[39])
    # FF8's Latin glyph encoding, verified against the vanilla Potion name.
    def encode(word):
        return bytes(ord(c) + 4 if c.isupper() else ord(c) - 2 if c.islower() else 0x20 for c in word) + b'\0'
    potion_name = struct.unpack_from('<H', sections[7], 24)[0]
    end = sections[38].index(b'\0', potion_name)
    if sections[38][potion_name:end + 1] != encode('Potion'):
        raise ValueError('This fixture generator expects English kernel text')
    name = len(text)
    text += encode('Extra Potion')
    desc = len(text)
    text += encode('Item expansion test')
    sections[39] = bytes(text)
    sections[8] += struct.pack('<HH', name, desc) * 57
    # Append-only battle format: record 33 maps to global ID 199, through 254.
    sections[7] += sections[7][24:48] * 56
    result = bytearray(struct.pack('<I', 56) + bytes(56 * 4))
    new_offsets = []
    for section in sections:
        new_offsets.append(len(result))
        result.extend(section)
    struct.pack_into('<56I', result, 4, *new_offsets)
    mitem, price = mitem_path.read_bytes(), price_path.read_bytes()
    if len(mitem) != 199 * 4 or len(price) != 199 * 4:
        raise ValueError('Expected vanilla 199-row menu files')
    output.mkdir(parents=True, exist_ok=True)
    (output / 'kernel.bin').write_bytes(result)
    (output / 'mitem.bin').write_bytes(mitem + mitem[4:8] * 57)
    (output / 'price.bin').write_bytes(price + price[4:8] * 57)
    print(f'Created {output}: 256 item definitions, 89 battle records, 256 menu/price rows')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('kernel', type=Path)
    parser.add_argument('mitem', type=Path)
    parser.add_argument('price', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    make_fixture(args.kernel, args.mitem, args.price, args.output)
