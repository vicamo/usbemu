/* usbemu - USB Emulation Library
 * Copyright (C) 2018 You-Sheng Yang
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#if defined (HAVE_CONFIG_H)
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linux/usbip.h>

#include <glib.h>
#include <gio/gio.h>
#include <gudev/gudev.h>

#include "usbemu/usbemu-errors.h"
#include "usbemu/usbemu-vhci-device.h"

#include "usbemu/internal/utils.h"
#include "usbemu/internal/vhci.h"

/**
 * SECTION:vhci
 * @short_description: internal functions for libusbemu
 * @stability: internal
 *
 * Internal used functions. Mainly protected member functions.
 */

#define LOG_COMPONENT "VHCI: "

#define SYSFS_VHCI_SUBSYSTEM "platform"
#define SYSFS_VHCI_DEVICE "vhci_hcd.0"

#define SYSFS_BUS_ID_SIZE 32

typedef struct _VhciPortStatus
{
  enum usb_device_speed hub;
  guint port;
  enum usbip_device_status status;
  enum usb_device_speed speed;
  guint32 devid;
  guint64 socket;
  gchar *busid;
} VhciPortStatus;

typedef struct _VhciContext
{
  GUdevClient *udev_client;
  GUdevDevice *device;
  GArray *statuses;
} VhciContext;

static void
_vhci_port_status_free (VhciPortStatus *port_status)
{
  if (port_status->busid != NULL)
    g_free (port_status->busid);
}

static void
_vhci_context_close (VhciContext *context)
{
  if (context->statuses != NULL)
    g_array_unref (context->statuses);
  if (context->device != NULL)
    g_object_unref (context->device);
  if (context->udev_client != NULL)
    g_object_unref (context->udev_client);
  g_slice_free (VhciContext, context);
}

static VhciContext*
_vhci_context_open (GError **error)
{
  GUdevClient *udev_client;
  VhciContext *context;

  udev_client = g_udev_client_new (NULL);
   if (udev_client == NULL) {
    g_set_error_literal (error, USBEMU_ERROR, USBEMU_ERROR_FAILED,
                         "unable to open udev");
    return NULL;
  }

  context = g_slice_new0 (VhciContext);
  context->udev_client = udev_client;
  context->device =
        g_udev_client_query_by_subsystem_and_name (context->udev_client,
                                                   SYSFS_VHCI_SUBSYSTEM,
                                                   SYSFS_VHCI_DEVICE);
  if (context->device == NULL) {
    g_set_error_literal (error, USBEMU_ERROR, USBEMU_ERROR_FAILED,
                         "unable to open kernel VHCI sysfs device");
    _vhci_context_close (context);
    return NULL;
  }

  context->statuses = g_array_new (FALSE, TRUE, sizeof (VhciPortStatus));
  g_array_set_clear_func (context->statuses,
                          (GDestroyNotify) _vhci_port_status_free);

  return context;
}

static void
parse_status (GArray      *statuses,
              const gchar *content)
{
  VhciPortStatus *port_status;
  gchar **lines, **line;
  gchar hub[3], busid[SYSFS_BUS_ID_SIZE];
  guint port, status, speed;
  guint32 devid;
  guint64 socket;
  gint ret;

  line = lines = g_strsplit (content, "\n", 0);
  if (*line == NULL) {
    g_strfreev (lines);
    return;
  }

  ++line; /* skip header */
  for (; *line != NULL; ++line) {
    ret = sscanf (*line, "%2s  %u %u %u %x %16lx %31s",
                  hub, &port, &status, &speed, &devid, &socket, busid);
    if (ret < 7)
      continue;

    if (port >= statuses->len) {
      g_warning (LOG_COMPONENT "got port %u >= nports %u", port, statuses->len);
      g_array_set_size (statuses, port + 1);
    }

    port_status = &g_array_index (statuses, VhciPortStatus, port);
    port_status->hub = (hub[0] == 'h' ? USB_SPEED_HIGH : USB_SPEED_SUPER);
    port_status->port = port;
    port_status->status = status;
    port_status->speed = speed;
    port_status->devid = devid;
    port_status->socket = socket;
    port_status->busid = g_strdup (busid);
    /* g_debug (LOG_COMPONENT "VHCI hub %s port %04u status %03u speed %03u"
             " dev %08u socket %016u busid %s",
             hub, port, status, speed, devid, socket, busid); */
  }

  g_strfreev (lines);
}

static void
_vhci_context_refresh_statuses (VhciContext *context)
{
  gchar attr[32];
  const gchar *value;
  guint nports, i;

  nports = g_udev_device_get_sysfs_attr_as_int (context->device, "nports");
  g_array_set_size (context->statuses, 0);
  g_array_set_size (context->statuses, nports);

  for (i = 0; TRUE; i++) {
    if (i > 0)
      snprintf (attr, sizeof (attr) - 1, "status.%u", i);
    else
      g_strlcpy (attr, "status", sizeof (attr));

    value = g_udev_device_get_sysfs_attr (context->device, attr);
    if (value == NULL)
      break;

    parse_status (context->statuses, value);
  }
}

static guint
_vhci_context_find_free_port_for_speed (VhciContext           *context,
                                        enum usb_device_speed  speed)
{
  VhciPortStatus *port_status;
  guint port;

  for (port = 0; port < context->statuses->len; port++) {
    port_status = &g_array_index (context->statuses, VhciPortStatus, port);
    if ((speed == USB_SPEED_SUPER) && (port_status->hub != USB_SPEED_SUPER))
      continue;

    if (port_status->status != VDEV_ST_NULL)
      continue;

    break;
  }

  return port;
}

G_GNUC_INTERNAL gboolean
_vhci_attach_device (guint                  *port,
                     gint                    sockfd,
                     guint32                 devid,
                     enum usb_device_speed   speed,
                     GError                **error)
{
  VhciContext *context;
  VhciPortStatus *port_status;
  guint p;
  gchar *buf, *path;
  gboolean ret;

  g_assert (port != NULL);
  g_assert (sockfd >= 0);
  g_assert (speed == USB_SPEED_HIGH || speed == USB_SPEED_SUPER);

  context = _vhci_context_open (error);
  if (context == NULL)
    return FALSE;

  _vhci_context_refresh_statuses (context);

#define SET_ERROR_AND_RETURN(code, ...) { \
        g_set_error (error, USBEMU_ERROR, code, __VA_ARGS__); \
        _vhci_context_close (context); \
        return FALSE; \
      }

  if (*port == USBEMU_VHCI_DEVICE_PORT_UNSPECIFIED) {
    p = _vhci_context_find_free_port_for_speed (context, speed);
    if (p >= context->statuses->len)
      SET_ERROR_AND_RETURN (USBEMU_ERROR_RESOURCE_UNAVAILABLE,
                            "all ports in use");

    *port = p;
  }

  if (*port >= context->statuses->len)
    SET_ERROR_AND_RETURN (USBEMU_ERROR_FAILED, "invalid port %u", *port);

  port_status = &g_array_index (context->statuses, VhciPortStatus, *port);
  if (port_status->status != VDEV_ST_NULL)
    SET_ERROR_AND_RETURN (USBEMU_ERROR_RESOURCE_UNAVAILABLE,
                          "port %u in use", *port);

  if ((speed == USB_SPEED_SUPER) && (port_status->hub != USB_SPEED_SUPER)) {
    SET_ERROR_AND_RETURN (USBEMU_ERROR_FAILED,
                          "speed %u requested but port %u has hub speed %u",
                          speed, *port, port_status->hub);
  }

#undef SET_ERROR_AND_RETURN

  buf = g_strdup_printf ("%u %d %u %u", *port, sockfd, devid, speed);
  path = g_build_filename (g_udev_device_get_sysfs_path (context->device),
                           "attach", NULL);
  ret = _usbemu_utils_write_sysfs_path (path, buf, strlen (buf), error);
  g_free (buf);
  g_free (path);

  _vhci_context_close (context);
  return ret;
}

G_GNUC_INTERNAL gboolean
_vhci_detach_device (guint    port,
                     GError **error)
{
  VhciContext *context;
  gchar *buf, *path;
  gboolean ret;

  context = _vhci_context_open (error);
  if (context == NULL)
    return FALSE;

  buf = g_strdup_printf ("%u", port);
  path = g_build_filename (g_udev_device_get_sysfs_path (context->device),
                           "detach", NULL);
  ret = _usbemu_utils_write_sysfs_path (path, buf, strlen (buf), error);
  g_free (buf);
  g_free (path);

  _vhci_context_close (context);
  return ret;
}
