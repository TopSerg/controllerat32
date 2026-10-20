"""Inspect C28x flash and the two RAM code copies; offline only.

RAM load addresses/counts are taken from the explicit memcpy calls at
v1 0x3E7DA6..0x3E7DBB and v2 0x3E5B5B..0x3E5B70.
"""
import argparse
import json
import re
import struct
import subprocess
from pathlib import Path

from disassemble_sintech_firmware import ROOT, REF, BIN

OUT = REF / 'static_analysis/c28x'
COPIES = {1: [(0x3ED84E, 0xC000, 0x24A3), (0x3ED0C8, 0x9000, 0x785)],
          2: [(0x3EB22A, 0xC000, 0x24A3), (0x3ED6CE, 0x9000, 0x785)]}


def make_ram():
    manifest = []
    for version, copies in COPIES.items():
        source = next(REF.rglob(f'SinTechController_v{version}.bin'))
        data = source.read_bytes()
        for load, run, length in copies:
            prefix = OUT / f'v{version}_ram_{run:04x}'
            fragment = data[2*(load-0x3E4000):2*(load-0x3E4000+length)]
            assert len(fragment) == 2*length
            words = struct.unpack(f'<{length}H', fragment)
            prefix.with_suffix('.asm').write_text(' .sect ".text"\n .global _start\n_start:\n' +
                '\n'.join(' .word ' + ','.join(f'0x{x:04X}' for x in words[n:n+8])
                          for n in range(0, length, 8)), encoding='ascii')
            prefix.with_suffix('.cmd').write_text(
                f'MEMORY {{ CODE : origin = 0x{run:X}, length = 0x{length:X} }}\n'
                'SECTIONS { .text : > CODE }\n', encoding='ascii')
            with prefix.with_suffix('.build.log').open('w') as log:
                subprocess.run([str(BIN/'cl2000.exe'), '--compile_only', '--float_support=fpu32',
                                '--obj_directory='+str(OUT), str(prefix.with_suffix('.asm'))],
                               stdout=log, stderr=log, check=True, cwd=OUT)
                subprocess.run([str(BIN/'lnk2000.exe'), '--entry_point=_start',
                                '--output_file='+str(prefix.with_suffix('.out')),
                                str(prefix.with_suffix('.cmd')), str(prefix.with_suffix('.obj'))],
                               stdout=log, stderr=log, check=True, cwd=OUT)
            with prefix.with_suffix('.dis').open('w') as output:
                subprocess.run([str(BIN/'dis2000.exe'), '-i', '-e', str(prefix.with_suffix('.out'))],
                               stdout=output, check=True, cwd=OUT)
            manifest.append(dict(version=version, load_word_address=hex(load),
                                 run_word_address=hex(run), word_count=length,
                                 listing=str(prefix.with_suffix('.dis').relative_to(ROOT))))
    (OUT/'ram_copies.json').write_text(json.dumps(manifest, indent=2))
    print(json.dumps(manifest, indent=2))


def instructions(version=1, ram=False):
    paths = ([OUT/f'v{version}_ram_{address:04x}.dis' for address in (0x9000,0xC000)]
             if ram else [OUT/f'SinTechController_v{version}.dis'])
    for path in paths:
        records = []
        for line in path.read_text().splitlines():
            match = re.match(r'^([0-9a-f]{8})\s+([0-9a-f]{4})\s+(.+)$', line)
            if match:
                records.append([int(match[1],16), match[3].strip(), line])
            elif re.match(r'^\s+\|\|', line) and records:
                records[-1][1] += ' ' + line.strip()
                records[-1][2] += '\n' + line
        yield from records


def dump(start, end, version=1, ram=False):
    for pc, ins, line in instructions(version, ram):
        if start <= pc < end:
            print(line)


def refs(address, version=1, ram=False):
    # This is a conservative linear hint, not CFG or full pointer analysis.
    dp = None
    for pc, ins, line in instructions(version, ram):
        match = re.search(r'MOV[WZ]+\s+DP, #0x([0-9a-f]+)', ins)
        if match:
            dp = int(match[1],16)
        direct = [] if dp is None else [dp*64+int(x,16) for x in re.findall(r'@0x([0-9a-f]+)',ins)]
        if address in direct or re.search(r'#0x0*'+f'{address:x}'+r'\b', ins):
            print(line, '; direct DP refs:', ','.join(hex(x) for x in direct))
        if re.match(r'(LCR|LRETR|IRET)\s', ins):
            dp = None


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=['make-ram','dump','refs'])
    parser.add_argument('start', nargs='?', type=lambda x:int(x,0))
    parser.add_argument('end', nargs='?', type=lambda x:int(x,0))
    parser.add_argument('--version', type=int, choices=[1,2], default=1)
    parser.add_argument('--ram', action='store_true')
    args = parser.parse_args()
    if args.command == 'make-ram': make_ram()
    elif args.command == 'dump': dump(args.start,args.end,args.version,args.ram)
    else: refs(args.start,args.version,args.ram)
