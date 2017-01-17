/**
 * @file modeset.c
 * @Brief  Fuctions for retrieving and setting modes through DRM
 * @author Bram Vlerick
 * @version 1.0
 * @date 2017-01-17
 */

#include "modeset.h"
static const char * const drm_states[] = {
	"None",
	"Connected",
	"Disconnected",
	"Unkown connection",
};

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
 * @Brief  Helper function to fill in the modes into the drm_mode_obj struct
 *
 * @Param conn The connection from which we will take the modes
 * @Param obj The object that will contain the copied list
 *
 * @Returns   0 if success, -1 if failed
 */
/* ---------------------------------------------------------------------------*/
static int retrieve_drm_modes(drmModeConnector *conn, struct drm_mode_obj *obj)
{
	int i;
	drmModeModeInfo mode;
	if (!conn || !obj) return -1;

	obj->modes = malloc(conn->count_modes * sizeof(drmModeModeInfo));
	if (!obj->modes) {
		logger_log(LOG_LVL_ERROR, "Failed to create modes object");
		return -1;
	}
	if (conn->count_modes == 0) {
		logger_log(LOG_LVL_WARNING,
			   "No modes available for connector");
		return 0;
	}

	memset(obj->modes,0,(conn->count_modes * sizeof(drmModeModeInfo)));
	memcpy(obj->modes,conn->modes,
	       (conn->count_modes * sizeof(drmModeModeInfo)));
	obj->nr_of_modes = conn->count_modes;

#ifdef DEBUG
	for (i = 0; i < conn->count_modes; i++) {
		mode = obj->modes[i];
		printf("%s\n",mode.name);
		//printf("%dx%d - %dhz\n",mode.hdisplay, mode.vdisplay,
		//       mode.vrefresh);
	}
#endif
	return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @Brief  Retrieve a resource pointer from the drm subsystem
 *
 * @Param fd file discriptor to the graphics card
 *
 * @Returns   NULL if failed, an allocated resource if successfull
 *
 * @Note User must clean up the memory!
 */
/* ---------------------------------------------------------------------------*/
static drmModeRes *retrieve_drm_resources(int *fd)
{
	uint64_t has_dumb;
	drmModeRes *resource = NULL;

	if (drmGetCap(*fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
		logger_log(LOG_LVL_ERROR, "DUMB Buffers not supported");
		return NULL;
	}
	resource = drmModeGetResources(*fd);
	if (!resource) {
		logger_log(LOG_LVL_ERROR, "Failed to retrieve resource");
		return NULL;
	}
	return resource;
}

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
struct drm_mode_obj *populate_drm_conn_list(char *device_name)
{
	int fd,i;
	struct drm_mode_obj *head = NULL;
	struct drm_mode_obj *new,*tmp = NULL;
	drmModeRes *resource = NULL;
	drmModeConnector *conn = NULL;

	fd = open(device_name, O_RDWR | O_CLOEXEC);
	if (!fd) {
		logger_log(LOG_LVL_ERROR, "Failed to open device");
		goto end;
	}

	resource = retrieve_drm_resources(&fd);
	if (!resource) {
		goto end;
	}

	for (i = 0; i < resource->count_connectors; i++) {
		/* Retrieve connector */
		conn = drmModeGetConnector(fd, resource->connectors[i]);
		if (!conn) {
			logger_log(LOG_LVL_ERROR,
				   "Failed to retrieve connector");
			continue;
		}

		new = malloc(sizeof(*new));
		memset(new,0,sizeof(*new));
		new->id = i;

		/* TODO: make card name dynamic by using device_name */
		snprintf(new->name,256,"Card0-%s-%d",
			 drm_output_names[conn->connector_type],
			 conn->connector_type_id);
#ifdef DEBUG
		fprintf(stdout,"Connector: %s is %s\n",new->name,
			drm_states[conn->connection]);
#endif
		retrieve_drm_modes(conn, new);

		/* Cleanup drm connector */
		if (conn) drmModeFreeConnector(conn);
		/* Set head of list */
		if (head == NULL)
			head = new;
		/* If tmp is set, link next ptr to current item */
		if (tmp)
			tmp->next = new;
		/* Set prev ptr of new to last saved item */
		new->prev = tmp;
		/* Update tmp */
		tmp = new;
	}

end:	if (resource) drmModeFreeResources(resource);
	if (fd) close(fd);
	return head;
}