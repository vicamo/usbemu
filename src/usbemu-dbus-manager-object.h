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

#pragma once

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define USBEMU_TYPE_DBUS_MANAGER_OBJECT  (usbemu_dbus_manager_object_get_type ())

G_DECLARE_DERIVABLE_TYPE (UsbemuDBusManagerObject, usbemu_dbus_manager_object,
                          USBEMU, DBUS_MANAGER_OBJECT, GDBusObjectSkeleton)

struct _UsbemuDBusManagerObjectClass {
  GDBusObjectSkeletonClass parent_class;

  /*< private >*/
  gpointer padding[12];
};

UsbemuDBusManagerObject* usbemu_dbus_manager_object_new ();

gboolean usbemu_dbus_manager_object_start (UsbemuDBusManagerObject *manager,
                                           GDBusConnection         *connection);

GList*
usbemu_dbus_manager_object_get_devices (UsbemuDBusManagerObject *manager);

G_END_DECLS
