#!/usr/bin/env python


BASE = '/home/apoorve/project'
DMTCP = BASE + '/dmtcp'
SPLASH = BASE + '/splash2_bench'
SPLASH_KERNEL = SPLASH + '/codes/kernels'
SPLASH_APPS = SPLASH = '/codes/apps'
PROBE = BASE + '/stencilprobe_bench'
STRACE = BASE + '/strace'

LOGDIR = BASE + '/log'
DATADIR = BASE + '/common/data'
CHKPTDIR = DATADIR + '/init_chkpts'
STATSDIR = DATADIR + '/stats'

DATAFILE = DATADIR + '/appdata.xml'
PERMUTATIONFILE = DATADIR + '/permutations.xml'
MYPLUG = DMTCP + '/src/plugin/myplug/dmtcp_myplughijack.so'

#DMTCP COMMANDS
DMTCP_COORDINATOR = DMTCP + '/bin/dmtcp_coordinator'
DMTCP_LAUNCH = DMTCP + '/bin/dmtcp_launch'
DMTCP_RESTART = DMTCP + '/bin/dmtcp_restart'
DMTCP_COMMAND = DMTCP + '/bin/dmtcp_command'

#APPLICATIONS COMMANDS

APP_PATH_MAP = {'RADIX' : SPLASH_KERNEL + '/radix', 'LUC' : SPLASH_KERNEL + '/lu/contiguous_blocks', 'LUNC' : SPLASH_KERNEL + '/lu/non_contiguous_blocks', 'FMM' : SPLASH_APPS + '/fmm/FMM', 'OCEANC' : SPLASH_APPS + '/ocean/contiguous_partitions', 'OCEANNC' : SPLASH_APPS + '/ocean/non_contiguous_partitions', 'WATERNSQUARED' : SPLASH_APPS + '/water-nsquared', 'PROBEBLOCKED' : PROBE, 'PROBEOBLIVIOUS' : PROBE}

#CONSTANTS
SHORTDELAY = 5
LONGDELAY = 30
LOGGER = 'default.log'

