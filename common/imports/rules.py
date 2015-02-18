#!/usr/bin/env python

#overcommit threads by 2X - min cache misses
def rule1(app_stats_map):
	import copy
	import rules_utils as ru
	import multithreading as m

	app_stats_map = copy.deepcopy(app_stats_map)
	next_runnable = []
	totalthreads = m.cpu_count() * 2
	toberemoved = None
	distance = 0

	while(totalthreads > 0) and (len(app_stats_map) > 0):
		app = get_app_with_min_cache_misses(app_stats_map)
		tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
		root = tree.getroot()
		thread = int(root.findall('THREADS')[0].text)
		next_runnable.append([app, thread])
		totalthreads -= thread
		distance = thread
		app_stats_map.pop(app)

	if totalthreads < 0:
		for app in next_runnable:
			if ((totalthreads + app[1]) < distance) and ((totalthreads + app[1]) >= 0)
				distance = totalthreads + app[1]
				toberemoved = next_runnable.index(app)

		next_runnable.pop(toberemoved)

	return [app[0] for app in next_runnable]

#no overcommit - min cache misses
def rule2(app_stats_map):
        import copy
        import rules_utils as ru
        import multithreading as m


        app_stats_map = copy.deepcopy(app_stats_map)
        next_runnable = []
        totalthreads = m.cpu_count()
        toberemoved = None
        distance = 0

        while(totalthreads > 0) and (len(app_stats_map) > 0):
                app = get_app_with_min_cache_misses(app_stats_map)
                tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
                root = tree.getroot()
                thread = int(root.findall('THREADS')[0].text)
                next_runnable.append([app, thread])
                totalthreads -= thread
                distance = thread
                app_stats_map.pop(app)

        if totalthreads < 0:
                for app in next_runnable:
                        if ((totalthreads + app[1]) < distance) and ((totalthreads + app[1]) >= 0)
                                distance = totalthreads + app[1]
                                toberemoved = next_runnable.index(app)

                next_runnable.pop(toberemoved)

        return [app[0] for app in next_runnable]


#total app mem < total sys mem
def rule3(app_stats_map):
	

#total app mem < total sys mem and no overcommit threads
def rule4(app_stats_map):

#total app mem < tota; sys mem and overcommit threads by 2X
def rule5(app_stats_map):

RULES = [rule1, rule2]
