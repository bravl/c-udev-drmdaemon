/**
 * @file modeset.h
 * @Brief  Retrieving and setting modes directly through DRM
 * @author Bram Vlerick
 * @version 1.0
 * @date 2017-01-16
 * TODO: Add way to retrieve current resolution
 * TODO: Add way to set resolution
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include "debug.h"

#define DEBUG

/* ---------------------------------------------------------------------------*/
/**
 * @Brief  Main drm connector structure
 */
/* ---------------------------------------------------------------------------*/
struct drm_mode_obj {
	/* Next and prev for Linked list structure */
	struct drm_mode_obj *next;
	struct drm_mode_obj *prev;

	/* Simple tracking id*/
	int id;

	/* DRM Defined connected, disconnected and error*/
	drmModeConnection status;
	/* DRM Node name e.g.: Card0-DP-1 */
	char name[256];
	/* Available resolutions */
	drmModeModeInfo *modes;
	int nr_of_modes;
	/* If connected, the current mode, if disconnected the last mode*/
	drmModeModeInfo last_mode;
};


/* ---------------------------------------------------------------------------*/
/**
 * @Brief  Populate the drm connector list.
 * This function populates the initial drm connector list. This list will
 * serve as a reference for following updates
 *
 * @Param device_name The device name (most cases: /dev/dri/card0)
 *
 * @Returns   The head of a linked list containing the available connectors
 */
/* ---------------------------------------------------------------------------*/
struct drm_mode_obj *populate_drm_conn_list(char *device_name);
