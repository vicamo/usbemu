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

#include <gio/gio.h>

G_BEGIN_DECLS

#define USBEMU_DBUS_TYPE_OBJECT_MANAGER_CLIENT (usbemu_dbus_object_manager_client_get_type ())
#define USBEMU_DBUS_OBJECT_MANAGER_CLIENT(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), USBEMU_DBUS_TYPE_OBJECT_MANAGER_CLIENT, UsbemuDBusObjectManagerClient))
#define USBEMU_DBUS_OBJECT_MANAGER_CLIENT_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), USBEMU_DBUS_TYPE_OBJECT_MANAGER_CLIENT, UsbemuDBusObjectManagerClientClass))
#define USBEMU_DBUS_OBJECT_MANAGER_CLIENT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), USBEMU_DBUS_TYPE_OBJECT_MANAGER_CLIENT, UsbemuDBusObjectManagerClientClass))
#define USBEMU_DBUS_IS_OBJECT_MANAGER_CLIENT(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), USBEMU_DBUS_TYPE_OBJECT_MANAGER_CLIENT))
#define USBEMU_DBUS_IS_OBJECT_MANAGER_CLIENT_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), USBEMU_DBUS_TYPE_OBJECT_MANAGER_CLIENT))

typedef struct _UsbemuDBusObjectManagerClient UsbemuDBusObjectManagerClient;
typedef struct _UsbemuDBusObjectManagerClientClass UsbemuDBusObjectManagerClientClass;
typedef struct _UsbemuDBusObjectManagerClientPrivate UsbemuDBusObjectManagerClientPrivate;

struct _UsbemuDBusObjectManagerClient
{
  /*< private >*/
  GDBusObjectManagerClient parent_instance;
  UsbemuDBusObjectManagerClientPrivate *priv;
};

struct _UsbemuDBusObjectManagerClientClass
{
  GDBusObjectManagerClientClass parent_class;
};

GType usbemu_dbus_object_manager_client_get_type (void) G_GNUC_CONST;

GType usbemu_dbus_object_manager_client_get_proxy_type (GDBusObjectManagerClient *manager, const gchar *object_path, const gchar *interface_name, gpointer user_data);

void usbemu_dbus_object_manager_client_new (
    GDBusConnection        *connection,
    GDBusObjectManagerClientFlags  flags,
    const gchar            *name,
    const gchar            *object_path,
    GCancellable           *cancellable,
    GAsyncReadyCallback     callback,
    gpointer                user_data);
GDBusObjectManager *usbemu_dbus_object_manager_client_new_finish (
    GAsyncResult        *res,
    GError             **error);
GDBusObjectManager *usbemu_dbus_object_manager_client_new_sync (
    GDBusConnection        *connection,
    GDBusObjectManagerClientFlags  flags,
    const gchar            *name,
    const gchar            *object_path,
    GCancellable           *cancellable,
    GError                **error);

void usbemu_dbus_object_manager_client_new_for_bus (
    GBusType                bus_type,
    GDBusObjectManagerClientFlags  flags,
    const gchar            *name,
    const gchar            *object_path,
    GCancellable           *cancellable,
    GAsyncReadyCallback     callback,
    gpointer                user_data);
GDBusObjectManager *usbemu_dbus_object_manager_client_new_for_bus_finish (
    GAsyncResult        *res,
    GError             **error);
GDBusObjectManager *usbemu_dbus_object_manager_client_new_for_bus_sync (
    GBusType                bus_type,
    GDBusObjectManagerClientFlags  flags,
    const gchar            *name,
    const gchar            *object_path,
    GCancellable           *cancellable,
    GError                **error);

G_END_DECLS
