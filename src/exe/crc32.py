#!/usr/bin/env python3
import sys, zlib

def calculate_crc(filename):
    crc = 0
    with open(filename, 'rb') as input_file:
        for block in input_file:
            crc = zlib.crc32(block, crc)
    return crc

if len(sys.argv) < 2:
    print(f'Usage: {sys.argv[0]} <filename>')
    sys.exit(1)

crc = calculate_crc(sys.argv[1])
print(f'{crc:08X}')
