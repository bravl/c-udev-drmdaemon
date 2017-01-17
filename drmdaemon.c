/**
 * @file drmdaemon.c
 * @Brief  Use DRM to control outputs and resolutions
 * Linking netsockets and DRM together to get dynamic resolution changes
 * @author Bram Vlerick
 * @version 1.0
 * @date 2017-01-16
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "debug.h"
#include "modeset.h"

//TODO: Change debug define with parameter parsing at boot
/* Uncomment to run without daemon and console logging */
#define DEBUG

static int daemonize()
{
	int x;
	pid_t pid;

	pid = fork();
	if (pid < 0) { /* Failed to create fork */
		logger_log(LOG_LVL_ERROR,"Failed to fork");
		return -1;
	}

	/* Gracefully exit parent */
	if (pid > 0)
		exit(EXIT_SUCCESS);

	/* Let child become session leader*/
	if (setsid() < 0)
		return -1;

	/* Ignore signals */
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	pid = fork();
	if (pid < 0) {
		logger_log(LOG_LVL_ERROR,
			   "Failed to create second fork");
		return -1;
	}

	/* Gracefully exit second parent */
	if (pid > 0)
		exit(EXIT_SUCCESS);
	umask(0);

	/* Close open file discriptors */
	for (x = sysconf(_SC_OPEN_MAX); x>=0; x--) {
		close(x);
	}
	return 0;
}

int main(int argc, char **argv)
{
#ifndef DEBUG
	daemonize();
	logger_set_file_logging("log.txt");
#endif
	logger_log(LOG_LVL_INFO, "Running drmdaemon");
	logger_log(LOG_LVL_INFO, "Creating daemon");
	struct drm_mode_obj *iter;
	struct drm_mode_obj *connectors = populate_drm_conn_list("/dev/dri/card1");
	if (!connectors) {
		logger_log(LOG_LVL_ERROR,"Failed to retrieve connectors");
		return -1;
	}

	//TODO: cleanup drm_mode_obj list properly
	return 0;
}