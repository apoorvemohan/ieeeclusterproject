#!/usr/bin/env python


def dmtcplaunch(app_id, plugin, newcoordinator, port, modifyenv, daemon):
	import constants
	import utils

	cmd = DMTCP_LAUNCH 

	if (plugin != None) and ('--with-plugin' in plugin):
		cmd += (' ' + plugin)
			if 'myplug' in cmd:
				cmd.replace('myplug', MYPLUG) 

	if (newcoordinator != None) and ('--new-coordinator' == newcoordinator):
		cmd += (' ' + newcoordinator)

	if (port != None) and ('--port' in port):
		cmd += (' ' + port)

	if (modifyenv != None) and ('--modify-env' == modifyenv):
		cmd += (' ' + modifyenv)

	cmd += (' ' + getappexeccommandbyid(app_id))

	if (background != None) and ('--daemon' == daemon):
		cmd += ' &'
	
	execcmd(cmd)
	
def dmtcpchkpt(port, blocked):
	import constants

	cmd = DMTCP_COMMAND

	if (blocked != None) and (blocked == '-b'):
		cmd += ' -bc'
	else:
		cmd += ' -c'

	if (port != None) and ('--port' in port):
		cmd += (' ' + port)

	execcmd(cmd)

def dmtcpkill(port):
	import constants

	cmd = DMTCP_COMMAND

	if (port != None) and ('--port' in port):
		cmd += (' ' + port)

	execcmd(cmd)
	
