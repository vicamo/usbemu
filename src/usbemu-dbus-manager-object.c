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

#include "config.h"

#include <usbemu/usbemu.h>
#include <usbemu/dbus/usbemu-dbus-manager.h>

#include "usbemu-dbus-manager-object.h"

typedef struct  _UsbemuDBusManagerObjectPrivate {
  GDBusObjectManagerServer *object_manager;

  UsbemuDBusManagerSkeleton *manager_skeleton;
} UsbemuDBusManagerObjectPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (UsbemuDBusManagerObject, usbemu_dbus_manager_object,
                            G_TYPE_DBUS_OBJECT_SKELETON)

#define USBEMU_DBUS_MANAGER_OBJECT_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), USBEMU_TYPE_DBUS_MANAGER_OBJECT, \
                                UsbemuDBusManagerObjectPrivate))

static void
constructed (GObject *object)
{
  UsbemuDBusManagerObject *manager = USBEMU_DBUS_MANAGER_OBJECT (object);
  UsbemuDBusManagerObjectPrivate *priv =
    USBEMU_DBUS_MANAGER_OBJECT_GET_PRIVATE (manager);
  UsbemuDBusManagerSkeleton *manager_skeleton;

  G_OBJECT_CLASS (usbemu_dbus_manager_object_parent_class)->constructed (object);

  /* setup UsbemuDBusManagerSkeleton */
  priv->manager_skeleton = manager_skeleton =
    (UsbemuDBusManagerSkeleton*) usbemu_dbus_manager_skeleton_new ();
  g_object_ref (priv->manager_skeleton);

  usbemu_dbus_manager_set_version (USBEMU_DBUS_MANAGER (manager_skeleton),
                                   USBEMU_VERSION);

  g_dbus_object_skeleton_add_interface (G_DBUS_OBJECT_SKELETON (manager),
                                        G_DBUS_INTERFACE_SKELETON (manager_skeleton));
}

static void
dispose (GObject *object)
{
  UsbemuDBusManagerObject *manager = USBEMU_DBUS_MANAGER_OBJECT (object);
  UsbemuDBusManagerObjectPrivate *priv =
    USBEMU_DBUS_MANAGER_OBJECT_GET_PRIVATE (manager);

  g_dbus_object_skeleton_remove_interface (G_DBUS_OBJECT_SKELETON (object),
                                           G_DBUS_INTERFACE_SKELETON (priv->manager_skeleton));
  g_object_unref (priv->manager_skeleton);
  priv->manager_skeleton = NULL;
}

static void
usbemu_dbus_manager_object_class_init (UsbemuDBusManagerObjectClass *manager_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (manager_class);

  /* virtual methods */
  object_class->constructed = constructed;
  object_class->dispose = dispose;
}

static void
usbemu_dbus_manager_object_init (UsbemuDBusManagerObject *manager G_GNUC_UNUSED)
{
  /* do nothing */
}

UsbemuDBusManagerObject*
usbemu_dbus_manager_object_new ()
{
  return g_object_new (USBEMU_TYPE_DBUS_MANAGER_OBJECT,
                       "g-object-path", USBEMU_DBUS_MANAGER_PATH,
                       NULL);
}

gboolean
usbemu_dbus_manager_object_start (UsbemuDBusManagerObject *manager,
                                  GDBusConnection *connection)
{
  UsbemuDBusManagerObjectPrivate *priv;

  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), FALSE);
  g_return_val_if_fail (USBEMU_IS_DBUS_MANAGER_OBJECT (manager), FALSE);

  priv = USBEMU_DBUS_MANAGER_OBJECT_GET_PRIVATE (manager);
  g_return_val_if_fail (priv->object_manager == NULL, FALSE);

  priv->object_manager = g_dbus_object_manager_server_new (USBEMU_DBUS_PATH);
  g_dbus_object_manager_server_export (priv->object_manager,
                                       G_DBUS_OBJECT_SKELETON (manager));

  g_dbus_object_manager_server_set_connection (priv->object_manager,
                                               connection);
  return TRUE;
}