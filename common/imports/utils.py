#!/usr/bin/env python

def execcmd(command):
	import os
	os.system(command)

def execcmdbg(command):
	import os
	os.system(command + ' &')

def mkdir(dirname):
	import os	
	os.mkdir(dirname)

def chdir(dirname):
	import os
	os.chdir(dirname)

def isfile(filename):
	import os
	return os.path.isfile(filename)

def isdir(dirname):
	import os
	os.path.isdir(dirname)

def gettimestamp():
	import datetime
	return str(datetime.datetime.now()).split('.')[0]

def getcwd():
	import os
	return os.getcwd()

def getopenport():
        import socket
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind(("",0))
        s.listen(1)
        port = s.getsockname()[1]
        s.close()
        return port

def getappidlist():
	import xml.etree.ElementTree as ET

	import constants

	tree = ET.parse(constants.DATAFILE)
	root = tree.getroot()
	return [attr.attrib.get('ID') for attr in root.iter('APPLICATIONDATA')]

def getappexeccommandbyid(app_id):
	import xml.etree.ElementTree as ET

        import constants

        tree = ET.parse(constants.DATAFILE)
        root = tree.getroot()
	app_name = None
	app_data = None

	for node in root.findall('APPLICATIONDATA'):
		if node.get('ID') == app_id:
			app_name = node.get('NAME')
			app_data = node
			break
	
	app_cmd = None

	for node in root.findall('APPLICATION'):
		if node.get('NAME') == app_name:
			app_cmd = node.text
			break

	app_cmd = app_cmd.replace(app_name+'_PATH', constants.APP_PATH_MAP[app_name])

	for node in app_data.getchildren():
		app_cmd = app_cmd.replace(node.tag, node.text)

	return app_cmd
