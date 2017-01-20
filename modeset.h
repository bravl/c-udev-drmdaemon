/**
 * @file modeset.h
 * @Brief  Retrieving and setting modes directly through DRM
 * @author Bram Vlerick
 * @version 1.0
 * @date 2017-01-16
 * TODO: Add way to retrieve current resolution: SEE TESTCODE FOR WORKING POC
 * TODO: Add way to set resolution
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <pthread.h>
#include "debug.h"

//#define DEBUG

static pthread_mutex_t _drm_obj_mutex;

/* ---------------------------------------------------------------------------*/
/**
 * @Brief  Lookup table for DRM connection status
 */
/* ---------------------------------------------------------------------------*/
static const char * const drm_states[] = {
	"None",
	"Connected",
	"Disconnected",
	"Unkown connection",
};

/* ---------------------------------------------------------------------------*/
/**
 * @Brief  Lookup table for DRM output names
 */
/* ---------------------------------------------------------------------------*/
static const char * const drm_output_names[] = {
	"None",
	"VGA",
	"DVI-I",
	"DVI-D",
	"DVI-A",
	"Composite",
	"SVIDEO",
	"LVDS",
	"Component",
	"DIN",
	"DP",
	"HDMI-A",
	"HDMI-B",
	"TV",
	"eDP",
	"Virtual",
	"DSI",
};

/* ---------------------------------------------------------------------------*/
/**
 * @Brief  Main drm connector structure
 */
/* ---------------------------------------------------------------------------*/
struct drm_connector_obj {
	/* Next and prev for Linked list structure */
	struct drm_connector_obj *next;
	struct drm_connector_obj *prev;

	/* Connector id*/
	uint32_t connector_id;

	/* The id of the connected crtc */
	uint32_t crtc_id;

	/* The id of the connected encoder */
	uint32_t encoder_id;

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
	drmModeModeInfo current_mode;
};

/* ---------------------------------------------------------------------------*/
/**
 * @Brief  Initialise the DRM handling lib
 *
 * @Returns   0 if successfull, -1 if failed
 */
/* ---------------------------------------------------------------------------*/
int init_drm_handler();

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
struct drm_connector_obj *populate_drm_conn_list(char *device_name);


/* ---------------------------------------------------------------------------*/
/**
 * @Brief  Update the current drm list with new values if something has changed
 *
 * @Param drm_connector_obj The head of the drm_connector_obj list
 * @Param device_name The device name of the card
 *
 * @Returns   number of changes
 */
/* ---------------------------------------------------------------------------*/
int update_drm_conn_list(struct drm_connector_obj *head, char *device_name);
