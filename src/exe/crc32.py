#!/usr/bin/env python3
import re, sys, zlib
from typing import Iterable

def calculate_crc(input_iter: Iterable[bytes], png: bool) -> int:
    crc = 0xFFFFFFFF if png else 0
    for block in input_iter:
        crc = zlib.crc32(block, crc)
    return 0xFFFFFFFF ^ crc if png else crc

def calculate_crc_name(filename: str, png: bool = False) -> int:
    if filename == '-':
        return calculate_crc(sys.stdin.buffer, png)
    with open(filename, 'rb') as input_file:
        return calculate_crc(input_file, png)

if len(sys.argv) < 2:
    print(f'Usage: {sys.argv[0]} <filename> [--png]', file=sys.stderr)
    sys.exit(1)

png_mode = len(sys.argv) >= 3 and sys.argv[2] == '--png'
filename = sys.argv[1]
crc = calculate_crc_name(filename, png_mode)
crc_str = f'{crc:08X}'
print(crc_str)
match = re.search(r'(?<![a-zA-Z0-9])([a-f0-9]{8}|[A-F0-9]{8})(?![a-zA-Z0-9])', filename)
if match:
    if match.group(0).upper() == crc_str:
        print(f'{filename} {crc_str} OK', file=sys.stderr)
    else:
        print(f'{filename} {crc_str} != {match.group(0).upper()} BAD', file=sys.stderr)
