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

#include <usbemu/usbemu-dbus-manager.h>

#include "usbemu-manager.h"

typedef struct  _UsbemuManagerPrivate {
  UsbemuDBusManagerSkeleton *manager_skeleton;
} UsbemuManagerPrivate;

/**
 * UsbemuManager:
 *
 * Server side GDBusObjectSkeleton instance for UsbemuDBusManager.
 */

/**
 * UsbemuManagerClass:
 * @parent_class: The parent class.
 * @padding: Reserved slots for furture extension.
 *
 * Class structure for UsbemuManager.
 */

G_DEFINE_TYPE_WITH_PRIVATE (UsbemuManager, usbemu_manager,
                            G_TYPE_DBUS_OBJECT_SKELETON)

#define USBEMU_MANAGER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), USBEMU_TYPE_MANAGER, UsbemuManagerPrivate))

static void
constructed (GObject *object)
{
  UsbemuManager *manager = USBEMU_MANAGER (object);
  UsbemuManagerPrivate *priv = USBEMU_MANAGER_GET_PRIVATE (manager);
  UsbemuDBusManagerSkeleton *manager_skeleton;

  G_OBJECT_CLASS (usbemu_manager_parent_class)->constructed (object);

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
  UsbemuManager *manager = USBEMU_MANAGER (object);
  UsbemuManagerPrivate *priv = USBEMU_MANAGER_GET_PRIVATE (manager);

  g_dbus_object_skeleton_remove_interface (G_DBUS_OBJECT_SKELETON (object),
                                           G_DBUS_INTERFACE_SKELETON (priv->manager_skeleton));
  g_object_unref (priv->manager_skeleton);
  priv->manager_skeleton = NULL;
}

static void
usbemu_manager_class_init (UsbemuManagerClass *manager_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (manager_class);

  /* virtual methods */
  object_class->constructed = constructed;
  object_class->dispose = dispose;
}

static void
usbemu_manager_init (UsbemuManager *manager G_GNUC_UNUSED)
{
  /* do nothing */
}

/**
 * usbemu_manager_new:
 * @object_path: An object path.
 *
 * Returns: (transfer full) (type UsbemuManager): The constructed manager object
 *          or %NULL.
 */
UsbemuManager*
usbemu_manager_new (const gchar *object_path)
{
  g_return_val_if_fail (g_variant_is_object_path (object_path), NULL);

  return g_object_new (USBEMU_TYPE_MANAGER, "g-object-path", object_path, NULL);
}
