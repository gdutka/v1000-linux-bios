#!/usr/bin/env python
#
# Copyright (C) 2022 Advanced Micro Devices, Inc. All rights reserved.
#
import os
import sys
# BaseTools/Source/Python/Split/Split.py
from Split import Split
FILE_DIR = os.path.dirname(__file__)
FV_PEI_SIZE = os.environ.get('FV_PEI_SIZE', '0xCFE00')
PERL_COMMAND = os.environ.get('PERL_COMMAND', 'perl')
COMPRESS_BIOS_SCRIPT_PATH = os.path.join(FILE_DIR, 'CompressBios.pl')
OutFile = sys.argv[3]
InFile = sys.argv[4]
OutFolder = os.path.dirname(OutFile)
Split.splitFile(InFile, 0x80, OutFolder, f'{InFile}.padding', f'{InFile}.PeiFv')
cmd = f'{PERL_COMMAND} {COMPRESS_BIOS_SCRIPT_PATH} {InFile}.PeiFv {OutFile} {FV_PEI_SIZE}'
print(cmd)
ret = os.system(cmd)
sys.exit(ret)

