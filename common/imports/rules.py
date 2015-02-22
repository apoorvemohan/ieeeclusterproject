#!/usr/bin/env python

#overcommit threads by 2X - min cache misses
def rule1(app_stats_map):
	print 'EXECUTING RULE1'
	import copy
	import rules_utils as ru
	import multiprocessing as m
	import xml.etree.ElementTree as ET
	import re

	import constants as c

	totalram = 0

	meminfo = open('/proc/meminfo').read()
	matched = re.search(r'^MemTotal:\s+(\d+)', meminfo)
	if matched: 
		totalram = int(matched.groups()[0])

	app_stats_map = copy.deepcopy(app_stats_map)
	next_runnable = []
	totalthreads = m.cpu_count() * 2
	toberemoved = None

#	while((totalram > 0) and (totalthreads > 0) and ((len(app_stats_map)) > 0)):
	while len(app_stats_map) > 0:
		app = ru.get_app_with_min_cache_misses(app_stats_map)
		tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
		root = tree.getroot()
		thread = int(root.findall('THREADS')[0].text)
		ram = int(app_stats_map['VmRSS'].split(' ')[0])
		if ((totalram - ram) > 0) and ((totalthreads - threads) > 0):
			totalram -= ram
			totalthreads -= thread
			next_runnable.append(app)
#		distance = thread
		app_stats_map.pop(app)

	return next_runnable

'''	distance = 0
	if totalram < 0:
                for app in next_runnable:
                        if ((totalram + app[2]) < distance) and ((totalram + app[2]) >= 0):
                                distance = totalram + app[2]
                                toberemoved = next_runnable.index(app)

		totalthreads += next_runnable[toberemoved][1]
                next_runnable.pop(toberemoved)
	

	distance = 0

	if totalthreads < 0:
		for app in next_runnable:
			if ((totalthreads + app[1]) < distance) and ((totalthreads + app[1]) >= 0):
				distance = totalthreads + app[1]
				toberemoved = next_runnable.index(app)

		next_runnable.pop(toberemoved)
'''
#	return [app[0] for app in next_runnable]


#no overcommit - min cache misses
def rule2(app_stats_map):
	print 'EXECUTING RULE2'
        import copy
        import rules_utils as ru
        import multiprocessing as m
        import xml.etree.ElementTree as ET
        import re

        import constants as c

        totalram = 0

        meminfo = open('/proc/meminfo').read()
        matched = re.search(r'^MemTotal:\s+(\d+)', meminfo)
        if matched:
                totalram = int(matched.groups()[0])

        app_stats_map = copy.deepcopy(app_stats_map)
        next_runnable = []
        totalthreads = m.cpu_count()
        toberemoved = None

#       while((totalram > 0) and (totalthreads > 0) and ((len(app_stats_map)) > 0)):
        while len(app_stats_map) > 0:
                app = ru.get_app_with_min_cache_misses(app_stats_map)
                tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
                root = tree.getroot()
                thread = int(root.findall('THREADS')[0].text)
                ram = int(app_stats_map['VmRSS'].split(' ')[0])
                if ((totalram - ram) > 0) and ((totalthreads - threads) > 0):
                        totalram -= ram
                        totalthreads -= thread
                        next_runnable.append(app)
#               distance = thread
                app_stats_map.pop(app)

        return next_runnable


#total app mem < total sys mem
def rule3(app_stats_map):
	print 'rule 3'

#total app mem < total sys mem and no overcommit threads
def rule4(app_stats_map):
	print 'rule4'

#total app mem < tota; sys mem and overcommit threads by 2X
def rule5(app_stats_map):
	print 'rule5'

RULES = {'rule1' : rule1, 'rule2' : rule2}
