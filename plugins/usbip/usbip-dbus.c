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

#include "usbemu/usbemu.h"
#include "usbemu/dbus/usbemu-dbus-object-manager.h"

#include "usbip.h"

gchar * opt_name = USBEMU_DBUS_PREFIX;
gboolean opt_session = FALSE;

void
usbip_dbus_init (const gchar         *address,
                 GDBusObjectManager **object_manager,
                 UsbemuDBusManager  **manager)
{
  GBusType type;
  GError *error = NULL;
  GDBusConnection *connection;
  GDBusObject *manager_object;

  g_assert (object_manager != NULL);
  g_assert (manager != NULL);

  type = opt_session ? G_BUS_TYPE_SESSION : G_BUS_TYPE_SYSTEM;

  if (address == NULL) {
    connection = g_bus_get_sync (type, NULL, &error);
  } else {
    gchar *addr;

    addr = g_dbus_address_get_for_bus_sync (type, NULL, &error);
    if (error != NULL) {
      g_printerr ("Failed to get address for %s type: %s\n",
                  (opt_session ? "session" : "system"), error->message);
      exit (1);
    }

    connection =
      g_dbus_connection_new_for_address_sync (addr,
                                              G_DBUS_CONNECTION_FLAGS_NONE,
                                              NULL, NULL, &error);
    g_free (addr);
  }

  if (error != NULL) {
    g_printerr ("Failed to create connection: %s\n", error->message);
    exit (1);
  }

  *object_manager =
    usbemu_dbus_object_manager_client_new_sync (connection,
                                                G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE,
                                                USBEMU_DBUS_PREFIX,
                                                USBEMU_DBUS_PATH,
                                                NULL,
                                                &error);
  if (error != NULL) {
    g_printerr ("Failed to create object manager: %s\n", error->message);
    exit (1);
  }

  manager_object = g_dbus_object_manager_get_object (*object_manager,
                                                     USBEMU_DBUS_MANAGER_PATH);
  if (manager_object == NULL) {
    g_printerr ("Failed to create manager: %s\n");
    exit (1);
  }

  *manager =
    USBEMU_DBUS_MANAGER (g_dbus_object_get_interface (manager_object,
                                                      USBEMU_DBUS_PREFIX));

  g_object_unref (connection);
  g_object_unref (manager_object);
}
