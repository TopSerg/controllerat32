"""Read-only PE inspection/disassembly helper; never loads the target EXE."""
import argparse
from pathlib import Path
import re
import struct
import sys

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'test_logs/sintech_reference_20261020/python_deps'))
import capstone
import pefile

P=ROOT/'test_logs/sintech_reference_20261020/software_files/BLDC_uart.exe'
DATA=P.read_bytes()
PE=pefile.PE(data=DATA)
BASE=PE.OPTIONAL_HEADER.ImageBase
MD=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
MD.skipdata=True
IMPORTS={f.address:f.name.decode(errors='replace') if f.name else str(f.ordinal)
         for dll in PE.DIRECTORY_ENTRY_IMPORT for f in dll.imports}

def off(va):
    return PE.get_offset_from_rva(va-BASE)

def va(offset):
    return BASE+PE.get_rva_from_offset(offset)

def dump(start,end):
    first=off(start)
    for address,size,mnemonic,operands in MD.disasm_lite(DATA[first:first+end-start],start):
        if mnemonic=='int3':
            continue
        note=''
        for value in re.findall(r'0x[0-9a-f]+',operands):
            number=int(value,16)
            if number in IMPORTS:
                note+=' ; '+IMPORTS[number]
        print(f'{address:08x} {mnemonic:8s} {operands}{note}')

def refs(target):
    pattern=struct.pack('<I',target)
    for m in re.finditer(re.escape(pattern),DATA):
        print(f'offset={m.start():08x} va={va(m.start()):08x}')

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('operation',choices=['dump','refs','all'])
    parser.add_argument('addresses',nargs='*',type=lambda value:int(value,0))
    args=parser.parse_args()
    if args.operation=='refs':
        for target in args.addresses:
            print(f'REFS {target:#x}')
            refs(target)
    elif args.operation=='dump':
        start=args.addresses[0]
        dump(start,args.addresses[1] if len(args.addresses)>1 else start+0x200)
    else:
        section=PE.sections[0]
        dump(BASE+section.VirtualAddress,BASE+section.VirtualAddress+section.Misc_VirtualSize)

if __name__=='__main__':
    main()
