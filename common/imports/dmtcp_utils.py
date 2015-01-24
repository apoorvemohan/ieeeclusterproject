#!/usr/bin/env python


def launch(app_id, plugin, newcoordinator, port, modifyenv, daemon):
	import constants
	import utils

	cmd = constants.DMTCP_LAUNCH 

	if (plugin != None) and ('--with-plugin' in plugin):
		cmd += (' ' + plugin)
		if 'myplug' in cmd:
			cmd.replace('myplug', constants.MYPLUG) 

	if (newcoordinator != None) and ('--new-coordinator' == newcoordinator):
		cmd += (' ' + newcoordinator)

	if (port != None) and ('--port' in port):
		cmd += (' ' + port)

	if (modifyenv != None) and ('--modify-env' == modifyenv):
		cmd += (' ' + modifyenv)

	cmd += (' ' + utils.getappexeccommandbyid(app_id))

	if (background != None) and ('--daemon' == daemon):
		utils.execcmdbg(cmd)
	else:
		utils.execcmd(cmd)
		
	
def chkpt(port, blocked):
	import constants
	import utils

	cmd = constants.DMTCP_COMMAND

	if (blocked != None) and (blocked == '-b'):
		cmd += ' -bc'
	else:
		cmd += ' -c'

	if (port != None) and ('--port' in port):
		cmd += (' ' + port)

	utils.execcmd(cmd)

def kill(port):
	import constants
	import utils

	cmd = constants.DMTCP_COMMAND

	if (port != None) and ('--port' in port):
		cmd += (' ' + port)

	utils.execcmd(cmd)
	
