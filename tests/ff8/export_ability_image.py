"""Export an open IDA database for the private FF8 ability regression harness.

Use a local database copy and pass a JSON anchor record measured in that
retail release. The record contains language, input_sha256, version_index,
and anchors: kernel_buffer, menu_use_items, sub_48b7e0, compute_char_stats,
magic_name_getter, gf_save, gf_runtime, fn_chara_ability_lists, menu_candidates.
The test enum indices are EN=0, JP=1, JP_NV=2, DE=3, ES=5, FR=7, IT=9.
No copyrighted game bytes belong in the repository.
"""
from pathlib import Path
import argparse
import hashlib
import json
import struct

import ida_bytes
import ida_nalt
import ida_segment
import ida_ua
import idautils
import idc


def export(config_path, output_dir):
    config = json.loads(Path(config_path).read_text())
    actual_hash = ida_nalt.retrieve_input_file_sha256().hex()
    if actual_hash != config['input_sha256']:
        raise ValueError('Anchor record belongs to a different executable')
    if ida_nalt.get_imagebase() != 0x400000:
        raise ValueError('Expected a 32-bit retail image based at 0x400000')
    output = Path(output_dir)
    output.mkdir(parents=True, exist_ok=True)
    prefix = output / ('FF8_' + config['language'])
    segments = [ida_segment.getseg(ea) for ea in idautils.Segments()]
    with prefix.with_suffix('.map').open('wb') as stream:
        stream.write(b'FF8MAP01' + struct.pack('<I', len(segments)))
        for segment in segments:
            size = segment.end_ea - segment.start_ea
            loaded = ida_bytes.get_bytes_and_mask(segment.start_ea, size)
            data = bytearray(loaded[0]) if loaded else bytearray(size)
            if loaded:
                for block, bits in enumerate(loaded[1]):
                    if bits == 255:
                        continue
                    start = block * 8
                    for bit in range(min(8, size-start)):
                        if not bits & (1 << bit):
                            data[start+bit] = 0
            stream.write(struct.pack('<II', segment.start_ea, size))
            stream.write(data)
    anchors = config['anchors']
    text = next(segment for segment in segments if ida_segment.get_segm_name(segment) == '.text')
    record = [config['version_index'], anchors['kernel_buffer'], anchors['menu_use_items'],
              anchors['sub_48b7e0'], anchors['compute_char_stats'], anchors['magic_name_getter'],
              anchors['gf_save'], anchors['gf_runtime'], text.end_ea, 0]
    prefix.with_suffix('.anchors').write_bytes(struct.pack('<10I', *record))
    command_list = ida_bytes.get_dword(anchors['fn_chara_ability_lists']+0x79)-1
    ranges = [(anchors['kernel_buffer']+0x40E0, anchors['kernel_buffer']+0x40E0+116*8),
              (command_list, command_list+136),
              (anchors['menu_candidates'], anchors['menu_candidates']+24)]
    data = ida_bytes.get_bytes(text.start_ea, text.end_ea-text.start_ea)
    top_bytes = {low >> 24 for low, high in ranges}
    operands = []
    for last in range(3, len(data)):
        if data[last] not in top_bytes:
            continue
        offset = last-3
        value = struct.unpack_from('<I', data, offset)[0]
        if not any(low <= value < high for low, high in ranges):
            continue
        address = text.start_ea+offset
        head = ida_bytes.get_item_head(address)
        instruction = ida_ua.insn_t()
        ida_ua.decode_insn(instruction, head)
        if any(operand.offb and head+operand.offb == address and operand.type in
               (ida_ua.o_mem, ida_ua.o_displ, ida_ua.o_imm) for operand in instruction.ops):
            operands.append(address)
    prefix.with_suffix('.operands').write_bytes(struct.pack('<I', len(operands)) +
                                               struct.pack('<'+str(len(operands))+'I', *operands))
    return {'language': config['language'], 'input_sha256': actual_hash,
            'operand_count': len(operands), 'map_sha256': hashlib.sha256(prefix.with_suffix('.map').read_bytes()).hexdigest()}


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--config', required=True)
    parser.add_argument('--output-dir', required=True)
    parser.add_argument('--exit', action='store_true', help='Close this IDA batch process after exporting')
    args = parser.parse_args(idc.ARGV[1:])
    print(json.dumps(export(args.config, args.output_dir), indent=2))
    if args.exit:
        idc.qexit(0)
