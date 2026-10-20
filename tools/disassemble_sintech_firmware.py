"""Offline C28x disassembly using an official local TI toolchain.

Wrap the unchanged downloaded image in a .text section at the flash word
address inferred independently from its OD pointers. Nothing is flashed.
"""
import hashlib
import json
import struct
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REF = ROOT / 'test_logs/sintech_reference_20261020'
BIN = REF / 'ti_tools/local/ti-cgt-c2000_19.6.0.STS/bin'


def main():
    out = REF / 'static_analysis/c28x'
    out.mkdir(exist_ok=True)
    report = []
    for path in sorted(REF.rglob('SinTechController*.bin')):
        data = path.read_bytes()
        words = struct.unpack(f'<{len(data)//2}H', data)
        asm = out / (path.stem + '.asm')
        asm.write_text(' .sect ".text"\n .global _firmware_start\n_firmware_start:\n' +
                       '\n'.join(' .word ' + ','.join(f'0x{v:04X}' for v in words[n:n+8])
                                 for n in range(0, len(words), 8)) + '\n', encoding='ascii')
        obj = asm.with_suffix('.obj')
        cmd = asm.with_suffix('.cmd')
        cmd.write_text('MEMORY { FLASH : origin = 0x3e4000, length = 0x10000 }\n'
                       'SECTIONS { .text : > FLASH }\n', encoding='ascii')
        image = asm.with_suffix('.out')
        with asm.with_suffix('.build.log').open('w', encoding='utf-8') as log:
            subprocess.run([str(BIN / 'cl2000.exe'), '--compile_only', '--silicon_version=28',
                            '--float_support=fpu32', '--obj_directory=' + str(out), str(asm)],
                           check=True, cwd=out, stdout=log, stderr=log)
            subprocess.run([str(BIN / 'lnk2000.exe'), '--entry_point=_firmware_start',
                            '--output_file=' + str(image), str(cmd), str(obj)],
                           check=True, cwd=out, stdout=log, stderr=log)
        listing = asm.with_suffix('.dis')
        with listing.open('w', encoding='utf-8') as dest:
            subprocess.run([str(BIN / 'dis2000.exe'), '-e', '-i', str(image)],
                           stdout=dest, check=True, cwd=out)
        report.append(dict(source=str(path.relative_to(ROOT)), sha256=hashlib.sha256(data).hexdigest(),
                           listing=str(listing.relative_to(ROOT)),
                           flash_word_base='0x3E4000', hardware_access=False,
                           caveat='Linear sweep includes data; validate branch targets and instruction boundaries.'))
        print(listing)
    (out / 'manifest.json').write_text(json.dumps(report, indent=2), encoding='utf-8')


if __name__ == '__main__':
    main()
