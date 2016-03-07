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

#include <stdlib.h>

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
  g_printerr ("Not implemented yet.\n");
  exit (1);
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
