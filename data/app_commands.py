#!/usr/bin/env python

import os
import sys

path = os.path.dirname(os.path.realpath(__file__))  + '/..'
sys.path.insert(0, path)

import common.imports.constants as c


SPLASH = c.BASE + '/splash2_bench'
SPLASH_KERNEL = SPLASH + '/codes/kernels'
SPLASH_APPS = SPLASH + '/codes/apps'
PROBE = c.BASE + '/stencilprobe_bench'

APP_CMD_MAP = {'RADIX' : SPLASH_KERNEL + '/radix/RADIX -pTHREADS -rBASE -nKEYS -mMAXVAL', 
		'LUC' : SPLASH_KERNEL + '/lu/contiguous_blocks/LUC -nMATRIXSIZE -pTHREADS -bBLOCKSIZE', 
		'LUNC' : SPLASH_KERNEL + '/lu/non_contiguous_blocks/LUNC -nMATRIXSIZE -pTHREADS -bBLOCKSIZE', 
		'FMM' : 'echo \'CLUSTERTYPE\nDISTRIBUTIONTYPE\nPARTICLES\nPRECISION\nTHREADS\nTIMESTEPS\nSTEPSURATION\nSOFTPARAM\nPARTSCHEME\n\'  | ' + SPLASH_APPS + '/fmm/FMM',
		'OCEANC' : SPLASH_APPS + '/ocean/contiguous_partitions/OCEANC -nGRIDSIZE -pTHREADS -eTOLERANCE -rDISTANCE -tTIMESTEPS',
		'OCEANNC' : SPLASH_APPS + '/ocean/non_contiguous_partitions/OCEANNC -nGRIDSIZE -pTHREADS -eTOLERANCE -rDISTANCE -tTIMESTEPS', 
		'WATERNSQUARED' :  'echo \'TSTEP NMOL NSTEP NORDER NSAVE NRST NPRINT NFMC THREADS CUTOFF\' |  ' + SPLASH_APPS  + '/water-nsquared/WATER-NSQUARED', 
		'PROBEBLOCKED' : PROBE + '/PROBEBLOCKED GRIDX GRIDY GRIDZ BLOCKX BLOCKY BLOCKZ TIMESTEPS', 
		'PROBEOBLIVIOUS' : PROBE + '/PROBEOBLIVIOUS GRIDX GRIDY GRIDZ BLOCKX BLOCKY BLOCKZ TIMESTEPS'}

