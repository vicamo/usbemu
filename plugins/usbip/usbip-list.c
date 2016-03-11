/* usbemu - USB Emulation Library
 * Copyright (C) 2016 You-Sheng Yang
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details:
 */

#if defined (HAVE_CONFIG_H)
#include "config.h"
#endif

#include <errno.h>
#include <stdlib.h>

#include <libudev.h>
#include <glib.h>
#include <gio/gio.h>

#include "usbemu/usbemu.h"
#include "usbemu/dbus/usbemu-dbus-device.h"

#include "usbip.h"

static gboolean opt_list_parsable = FALSE;
static const gchar *opt_list_remote_path;
static gboolean opt_list_remote = FALSE;
static gboolean opt_list_local = FALSE;

static gboolean
opt_list_remote_func (const gchar *option_name,
                      const gchar *value,
                      gpointer data,
                      GError **error)
{
  if ((value == NULL) || g_dbus_is_supported_address (value, error)) {
    opt_list_remote = TRUE;
    opt_list_remote_path = value;
    return TRUE;
  }

  return FALSE;
}

const GOptionEntry list_entries[] =
{
  { "parsable", 'p', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_list_parsable,
    "Parsable list format", NULL },
  { "remote", 'r', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK,
    opt_list_remote_func, "List the exportable USB devices on HOST", "HOST" },
  { "local", 'l', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_list_local,
    "List the local USB devices", NULL },
  { NULL }
};

static void
do_list_remote ()
{
  GDBusObjectManager *object_manager;
  UsbemuDBusManager *manager;
  GError *error = NULL;
  const gchar * const *list;
  const gchar * const *path;

  usbip_dbus_init (opt_list_remote_path, &object_manager, &manager);

  list = usbemu_dbus_manager_get_devices (manager);
  if (list == NULL || *list == NULL) {
    g_print ("no exportable devices found on %s\n",
             opt_list_remote_path ? opt_list_remote_path : "(local)");
    return;
  }

  g_print ("Exportable USB devices\n");
  g_print ("======================\n");
  g_print (" - %s\n\n", opt_list_remote_path ? opt_list_remote_path : "(local)");

  path = list;
  while (*path) {
    GDBusObject *device_object;
    UsbemuDBusDevice *device;

    device_object = g_dbus_object_manager_get_object (object_manager, *path);
    device =
      USBEMU_DBUS_DEVICE (g_dbus_object_get_interface (device_object,
                                                       USBEMU_DBUS_DEVICE_PREFIX));

    g_object_unref (device_object);

    g_print ("%s\n", *path);
    g_print ("\n");

    g_object_unref (device);

    ++path;
  }

  g_object_unref (manager);
  g_object_unref (object_manager);
}

static void
do_list_local ()
{
  struct udev *udev;
  struct udev_enumerate *enumerate;
  struct udev_list_entry *devices, *dev_list_entry;
  struct udev_device *dev;
  const gchar *path;
  const gchar *idVendor;
  const gchar *idProduct;
  const gchar *bConfValue;
  const gchar *bNumIntfs;
  const gchar *busid;

  /* Create libudev context. */
  udev = udev_new ();

  /* Create libudev device enumeration. */
  enumerate = udev_enumerate_new (udev);

  /* Take only USB devices that are not hubs and do not have
    * the bInterfaceNumber attribute, i.e. are not interfaces.
    */
  udev_enumerate_add_match_subsystem (enumerate, "usb");
  udev_enumerate_add_nomatch_sysattr (enumerate, "bDeviceClass", "09");
  udev_enumerate_add_nomatch_sysattr (enumerate, "bInterfaceNumber", NULL);
  udev_enumerate_scan_devices (enumerate);

  devices = udev_enumerate_get_list_entry (enumerate);

  /* Show information about each device. */
  udev_list_entry_foreach (dev_list_entry, devices) {
    path = udev_list_entry_get_name (dev_list_entry);
    dev = udev_device_new_from_syspath (udev, path);

    /* Get device information. */
    idVendor = udev_device_get_sysattr_value (dev, "idVendor");
    idProduct = udev_device_get_sysattr_value (dev, "idProduct");
    bConfValue = udev_device_get_sysattr_value (dev, "bConfigurationValue");
    bNumIntfs = udev_device_get_sysattr_value (dev, "bNumInterfaces");
    busid = udev_device_get_sysname (dev);
    if (!idVendor || !idProduct || !bConfValue || !bNumIntfs) {
      g_printerr ("problem getting device attributes: %s\n",
                  g_strerror (errno));
      break;
    }

    g_print (" - busid %s (%.4s:%.4s)\n\n", busid, idVendor, idProduct);

    udev_device_unref (dev);
  }

  udev_enumerate_unref (enumerate);
  udev_unref (udev);
}

void
do_list (int   argc,
         char *argv[])
{
  if (opt_list_remote) {
    do_list_remote ();
  } else {
    do_list_local ();
  }
}
