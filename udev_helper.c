/**
 * @file udev_helper.c
 * @Brief  Udev helper functions
 * @author Bram Vlerick
 * @version 1.0
 * @date 2017-01-18
 */

#include "udev_helper.h"

/* ---------------------------------------------------------------------------*/
/**
 * @Brief  Setup a new udev monitor for a given subsystem
 *
 * @Param udev Pointer to the udev instance
 * @Param subsystem name of the subsystem we want to monitor
 *
 * @Returns   A newly created monitor or NULL if it has failed
 */
/* ---------------------------------------------------------------------------*/
struct udev_monitor *setup_udev_monitor(struct udev *udev, char *subsystem)
{
	struct udev_monitor *mon = NULL;
	mon = udev_monitor_new_from_netlink(udev, "udev");
	if (!mon) {
		logger_log(LOG_LVL_ERROR, "Failed to create monitor");
		return NULL;
	}
	if (udev_monitor_filter_add_match_subsystem_devtype(
		mon, subsystem, NULL) < 0) {
		logger_log(LOG_LVL_ERROR, "Failed to setup filter");
		return NULL;
	}
	if (udev_monitor_filter_update(mon) < 0) {
		logger_log(LOG_LVL_ERROR, "Unable to update monitor");
		return NULL;
	}
	if (udev_monitor_enable_receiving(mon) < 0) {
		logger_log(LOG_LVL_ERROR, "Unable to enable receiving");
		return NULL;
	}
	return mon;
}
