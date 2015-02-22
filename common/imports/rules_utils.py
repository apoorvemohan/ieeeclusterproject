#!/usr/bin/env python

def get_app_with_min_cache_misses(app_stats_map):
	minkey = app_stats_map.keys()[0]
	min_miss = int(app_stats_map[minkey]['CACHE_MISSES'])

	for app in app_stats_map.keys():
		if int(app_stats_map[app]['CACHE_MISSES']) < min_miss:
			minkey = app

	return minkey

