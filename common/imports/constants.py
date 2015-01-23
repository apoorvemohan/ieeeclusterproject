#!/usr/bin/env python

#PATH
BASE = '/home/apoorve/project'
DMTCP = BASE + '/dmtcp'
SPALSH = BASE + '/splash2_bench'
SPLASH_KERNEL = SPLASH + '/codes/kernel'
SPLASH_APPS = SPLASH = '/codes/apps'
PROBE = BASE + '/stencilprobe_bench'
LOG = BASE + '/log'
STRACE = BASE + '/strace'

#DMTCP COMMANDS
DMTCP_COORDINATOR = DMTCP + '/bin/dmtcp_coordinator'
DMTCP_LAUNCH = DMTCP + '/bin/dmtcp_launch'
DMTCP_RESTART = DMTCP + '/bin/dmtcp_restart'
DMTCP_COMMAND = DMTCP + '/bin/dmtcp_command'

#APPLICATIONS COMMANDS
RADIX = SPLASH_KERNEL + '/radix/RADIX -pTHREADS -rRADIX -nKEYS -mMAXVAL'
LUC = SPLASH_KERNEL + '/lu/contiguous_blocks/LUC -nMATRIXSIZE -pTHREADS -bBLOCKSIZE'
LUNC = SPLASH_KERNEL + '/lu/non_contiguous_blocks/LUNC -nMATRIXSIZE -pTHREADS -bBLOCKSIZE'
FMM = "echo '\n'  | " + SPLASH_APPS + '/fmm/FMM'


#CONSTANTS

