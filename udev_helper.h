/**
 * @file udev_helper.h
 * @Brief  Header file for some UDev library helper functions
 * @author Bram Vlerick
 * @version 1.0
 * @date 2017-01-18
 */

#include <stdio.h>
#include <libudev.h>
#include <sys/select.h>
#include <stdlib.h>

#include "debug.h"


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
struct udev_monitor *setup_udev_monitor(struct udev *udev, char *subsystem);
