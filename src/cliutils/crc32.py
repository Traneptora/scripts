#!/usr/bin/env python

import zlib
import sys

def crc(fileName):
    prev = 0
    for eachLine in open(fileName, "rb"):
        prev = zlib.crc32(eachLine, prev)
    calculated_crc = "%X"%(prev & 0xFFFFFFFF)
    if len(calculated_crc) < 8:
        calculated_crc = ((8 - len(calculated_crc)) * "0") + calculated_crc
    return calculated_crc

if len(sys.argv) < 2:
	print("Usage: " + sys.argv[0] + " <filename>")
else:
	print(crc(sys.argv[1]))

