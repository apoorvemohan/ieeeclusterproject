#!/usr/bin/env python


def launch(app_cmd, plugin, newcoordinator, port, modifyenv, daemon):
	import utils
	import constants

	envvarmap = {'STATFILE' : '', 'STATGEN' : '', 'REMOVEFROMQUEUE' : ''}

	cmd = 'STATFILE=' + envvarmap['STATFILE'] + ' ' + 'STATGEN=' + envvarmap['STATGEN'] + ' ' + 'REMOVEFROMQUEUE=' + envvarmap['REMOVEFROMQUEUE'] + ' '

	cmd += constants.DMTCP_LAUNCH 

	if (plugin != None) and ('--with-plugin' in plugin):
		cmd += (' ' + plugin)
		if 'myplug' in cmd:
			cmd = cmd.replace('myplug', constants.MYPLUG) 

	if (newcoordinator != None) and ('--new-coordinator' == newcoordinator):
		cmd += (' ' + newcoordinator)

	if (port != None) and ('--port' in port):
		cmd += (' ' + port)

	if (modifyenv != None) and ('--modify-env' == modifyenv):
		cmd += (' ' + modifyenv)

	cmd += (' --no-gzip')

	cmd += (' ' + app_cmd)

	cmd += (' >> ' + constants.LOGDIR + '/' + constants.LOGGER + ' 2>&1 ')

	if (daemon != None) and ('--daemon' == daemon):
		utils.execcmdbg(cmd)
	else:
		utils.execcmd(cmd)
		
	
def chkpt(port, blocked):
	import utils
	import constants

	cmd = constants.DMTCP_COMMAND

	if (blocked != None) and (blocked == '-b'):
		cmd += ' -bc'
	else:
		cmd += ' -c'

	if (port != None) and ('--port' in port):
		cmd += (' ' + port)
	
	cmd += (' >> ' + constants.LOGDIR + '/' + constants.LOGGER + ' 2>&1 ')

	utils.execcmd(cmd)

def kill(port):
	import utils
	import constants

	cmd = constants.DMTCP_COMMAND + ' -k'

	if (port != None) and ('--port' in port):
		cmd += (' ' + port)

	cmd += (' >> ' + constants.LOGDIR + '/' + constants.LOGGER + ' 2>&1 ')

	utils.execcmd(cmd)

def restart(chkpt_img, newcoordinator, port, daemon, envvarmap):
	import utils
	import constants

	perf_stat = ''

	cmd = constants.DMTCP_RESTART

	if (newcoordinator != None) and ('--new-coordinator' == newcoordinator):
                cmd += (' ' + newcoordinator)

	if (port != None) and ('--port' in port):
                cmd += (' ' + port)

	cmd += (' ' + chkpt_img)

	if (daemon == None) and (envvarmap['STATGEN'] == 'ONCE'):
		perf_stat = ' perf stat '
		cmd += (' >> ' + envvarmap['STATFILE'] + ' 2>&1 ')
	else:
		cmd += (' >> ' + constants.LOGDIR + '/' + constants.LOGGER + ' 2>&1 ')


	cmd = 'STATFILE=' + envvarmap['STATFILE'] + ' ' + 'STATGEN=' + envvarmap['STATGEN'] + ' ' + 'REMOVEFROMQUEUE=' + envvarmap['REMOVEFROMQUEUE'] + ' ' + perf_stat + cmd

	if (daemon != None) and ('--daemon' == daemon):
                utils.execcmdbg(cmd)
        else:
                utils.execcmd(cmd)
