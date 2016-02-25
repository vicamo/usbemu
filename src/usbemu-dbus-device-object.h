/* usbemu - USB Emulation Library
 * Copyright (C) 2016 You-Sheng Yang
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

#pragma once

#include <glib-object.h>
#include <gio/gio.h>

#include <usbemu/usbemu.h>

G_BEGIN_DECLS

#define USBEMU_TYPE_DBUS_DEVICE_OBJECT  (usbemu_dbus_device_object_get_type ())

G_DECLARE_DERIVABLE_TYPE (UsbemuDBusDeviceObject, usbemu_dbus_device_object,
                          USBEMU, DBUS_DEVICE_OBJECT, GDBusObjectSkeleton)

#define USBEMU_DBUS_DEVICE_OBJECT_PROP_DEVICE "device"

struct _UsbemuDBusDeviceObjectClass {
  GDBusObjectSkeletonClass parent_class;

  /*< private >*/
  gpointer padding[12];
};

UsbemuDBusDeviceObject* usbemu_dbus_device_object_new (const gchar  *object_path,
                                                       UsbemuDevice *device);

UsbemuDevice* usbemu_dbus_device_object_get_device (UsbemuDBusDeviceObject *device);
UsbemuDevice* usbemu_dbus_device_object_dup_device (UsbemuDBusDeviceObject *device);

G_END_DECLS
