/**
 * @file modeset.c
 * @Brief  Functions for retrieving and setting modes through DRM
 * @author Bram Vlerick
 * @version 1.0
 * @date 2017-01-17
 */

#include "modeset.h"

/* ---------------------------------------------------------------------------*/
/**
 * @Brief  Retrieve the crtc mode that is in use for a given connector
 *
 * @Param res Pointer to the DRM resources
 * @Param fd File descriptor for the device
 * @Param crtc_id crtc_id of a connector
 *
 * @Returns   empty mode if not connected or found, current mode if found
 */
/* ---------------------------------------------------------------------------*/
static drmModeModeInfo retrieve_current_crtc_mode(drmModeRes *res, int fd,
						  uint32_t crtc_id)
{
	int i;
	drmModeModeInfo mode;
	memset(&mode, 0, sizeof(mode));
	for (i = 0; i < res->count_crtcs; i++) {
		drmModeCrtc *crtc = drmModeGetCrtc(fd, res->crtcs[i]);
		if (crtc->crtc_id == crtc_id) {
#ifdef DEBUG
			printf("Found match %dx%d\n",
			       crtc->mode.hdisplay,
			       crtc->mode.vdisplay);
#endif
			mode = crtc->mode;
			drmModeFreeCrtc(crtc);
			return mode;
		}
	}
	return mode;
}

/* ---------------------------------------------------------------------------*/
/**
 * @Brief  Retrieve the crtc id used by this connector
 *
 * @Param fd the device file discriptor
 * @Param conn drmConnector
 *
 * @Returns   -1 if failed otherwise the crtc id
 */
/* ---------------------------------------------------------------------------*/
static int retrieve_drm_crtc_id(int *fd, drmModeConnector *conn)
{
	int crtc_id = 0;
	drmModeEncoder *enc;
	if (!fd || !conn) {
		logger_log(LOG_LVL_ERROR, "Params cannot be NULL");
		return -1;
	}
	if (conn->count_encoders == 0 || conn->encoder_id == 0) {
		logger_log(LOG_LVL_WARNING,
			   "No encoders or invalid encoder id");
		logger_log(LOG_LVL_INFO, "Probably no display connected");
		return -1;
	}
	enc = drmModeGetEncoder(*fd, conn->encoders[0]);
	if (!enc) {
		logger_log(LOG_LVL_ERROR, "Failed to retrieve encoder");
		return -1;
	}
	crtc_id = enc->crtc_id;
	drmModeFreeEncoder(enc);
	return crtc_id;
}

/* ---------------------------------------------------------------------------*/
/**
 * @Brief  Helper function to fill in the modes into the drm_connector_obj
 * struct
 *
 * @Param conn The connection from which we will take the modes
 * @Param obj The object that will contain the copied list
 *
 * @Returns   0 if success, -1 if failed
 */
/* ---------------------------------------------------------------------------*/
static int retrieve_drm_modes(drmModeConnector *conn,
			      struct drm_connector_obj *obj)
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
		logger_log(LOG_LVL_WARNING, "No modes available for connector");
		return 0;
	}

	memset(obj->modes, 0, (conn->count_modes * sizeof(drmModeModeInfo)));
	memcpy(obj->modes,
	       conn->modes,
	       (conn->count_modes * sizeof(drmModeModeInfo)));
	obj->nr_of_modes = conn->count_modes;

#ifdef DEBUG
	for (i = 0; i < conn->count_modes; i++) {
		mode = obj->modes[i];
		printf("%s\n", mode.name);
		// printf("%dx%d - %dhz\n",mode.hdisplay, mode.vdisplay,
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

static int update_connector(int fd, drmModeConnector *conn, drmModeRes *res,
			    struct drm_connector_obj *obj)
{
	uint32_t tmpval = 0;
	drmModeModeInfo tmpMode;
	int updated = 0;

	logger_log(LOG_LVL_INFO, "Updating %s", obj->name);
	if (obj->status != conn->connection) {
		logger_log(LOG_LVL_INFO,
			   "Updating status: %s",
			   drm_states[conn->connection]);
		obj->status = conn->connection;
		updated = 1;
	}

	if (obj->status == DRM_MODE_CONNECTED) {
		if (obj->encoder_id != conn->encoder_id) {
			obj->connector_id = conn->encoder_id;
			logger_log(LOG_LVL_INFO,
				   "Updating encoder id %d",
				   obj->connector_id);
			updated = 1;
		}
		tmpval = retrieve_drm_crtc_id(&fd, conn);
		if (obj->crtc_id != tmpval) {
			logger_log(LOG_LVL_INFO, "Updating crtc id %d", tmpval);
			obj->crtc_id = tmpval;
			updated = 1;
		}
		tmpMode = retrieve_current_crtc_mode(res, fd, obj->crtc_id);
		if (strcmp(tmpMode.name, obj->current_mode.name)) {
			logger_log(LOG_LVL_INFO, "Updating current mode");
			obj->current_mode = tmpMode;
			updated = 1;
		}
	}
	return updated;
}

static int compare_and_update_connector(int fd, drmModeConnector *conn,
					drmModeRes *res,
					struct drm_connector_obj *head)
{
	int updated = 0, update_count = 0;
	struct drm_connector_obj *iter;
	for (iter = head; iter != NULL; iter = iter->next) {
		if (iter->connector_id == conn->connector_id) {
			logger_log(LOG_LVL_INFO, "Found connector");
			updated = update_connector(fd, conn, res, iter);
			if (updated) {
				logger_log(LOG_LVL_INFO, "Connector updated");
				update_count++;
			}
		}
	}
	return update_count;
}

/* ---------------------------------------------------------------------------*/
/**
 * @Brief  Initialise the DRM handling lib
 *
 * @Returns   0 if successfull, -1 if failed
 */
/* ---------------------------------------------------------------------------*/
int init_drm_handler()
{
	if (pthread_mutex_init(&_drm_obj_mutex, NULL) < 0) {
		logger_log(LOG_LVL_ERROR, "Failed to init mutex\n");
		return -1;
	}
	return 0;
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
struct drm_connector_obj *populate_drm_conn_list(char *device_name)
{
	int fd, i, retval;
	struct drm_connector_obj *head = NULL;
	struct drm_connector_obj *new, *tmp = NULL;
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
		memset(new, 0, sizeof(*new));
		new->id = i;
		new->connector_id = conn->connector_id;

		/* TODO: make card name dynamic by using device_name */
		snprintf(new->name,
			 256,
			 "Card0-%s-%d",
			 drm_output_names[conn->connector_type],
			 conn->connector_type_id);
#ifdef DEBUG
		fprintf(stdout,
			"Connector: %s is %s\n",
			new->name,
			drm_states[conn->connection]);
#endif
		/* Retrieve modes for this connector */
		if (conn->connection == DRM_MODE_CONNECTED) {
			if (retrieve_drm_modes(conn, new) < 0) {
				if (conn) drmModeFreeConnector(conn);
				if (new) free(new);
				continue;
			}
			if ((retval = retrieve_drm_crtc_id(&fd, conn)) < 0) {
				if (conn) drmModeFreeConnector(conn);
				if (new) free(new);
				continue;
			}
			new->crtc_id = retval;
			/* TODO: Add workaround for mode.name not filled in by
			 * amd */
			/* TODO: Fix the mode.name in the AMD kernel driver */
			new->current_mode = retrieve_current_crtc_mode(
			    resource, fd, new->crtc_id);
			logger_log(LOG_LVL_INFO,
				   "Current mode for %s: %s",
				   new->name,
				   new->current_mode.name);
		}
		/* Cleanup drm connector */
		if (conn) drmModeFreeConnector(conn);
		/* Set head of list */
		if (head == NULL) head = new;
		/* If tmp is set, link next ptr to current item */
		if (tmp) tmp->next = new;
		/* Set prev ptr of new to last saved item */
		new->prev = tmp;
		/* Update tmp */
		tmp = new;
	}

end:
	if (resource) drmModeFreeResources(resource);
	if (fd) close(fd);
	return head;
}

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
int update_drm_conn_list(struct drm_connector_obj *head, char *device_name)
{
	int fd, i, retval = 0;
	drmModeRes *resource = NULL;
	drmModeConnector *conn = NULL;

	fd = open(device_name, O_RDWR | O_CLOEXEC);
	if (!fd) {
		logger_log(LOG_LVL_ERROR, "Failed to open device");
		retval = -1;
		goto end;
	}

	resource = retrieve_drm_resources(&fd);
	if (!resource) {
		retval = -1;
		goto end;
	}

	logger_log(LOG_LVL_INFO, "Updating DRM connector list");
	for (i = 0; i < resource->count_connectors; i++) {
		conn = drmModeGetConnector(fd, resource->connectors[i]);
		retval = compare_and_update_connector(fd, conn, resource, head);
		drmModeFreeConnector(conn);
	}

end:
	if (resource) drmModeFreeResources(resource);
	return retval;
}
