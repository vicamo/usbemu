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

#include "usbemu/usbemu-manager.h"

/**
 * SECTION:usbemumanager
 * @Short_description: Management object.
 * @Title: Manager
 */

/**
 * UsbemuManager:
 *
 * An object maintaining USB device emulation.
 */

/**
 * UsbemuManagerClass:
 * @parent_class: The parent class.
 *
 * Class structure for UsbemuManager.
 */

typedef struct  _UsbemuManagerPrivate {
  GList *devices;
} UsbemuManagerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (UsbemuManager, usbemu_manager, G_TYPE_OBJECT)

#define USBEMU_MANAGER_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), USBEMU_TYPE_MANAGER, UsbemuManagerPrivate))

static void
dispose (GObject *object)
{
  UsbemuManager *manager = USBEMU_MANAGER (object);
  UsbemuManagerPrivate *priv = USBEMU_MANAGER_GET_PRIVATE (manager);

  g_assert (priv->devices == NULL);
}

static void
usbemu_manager_class_init (UsbemuManagerClass *manager_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (manager_class);

  /* virtual methods */
  object_class->dispose = dispose;
}

static void
usbemu_manager_init (UsbemuManager *manager G_GNUC_UNUSED)
{
  /* do nothing */
}

/**
 * usbemu_manager_new:
 *
 * Returns: (transfer full) (type UsbemuManager): The constructed manager object
 *          or %NULL.
 */
UsbemuManager*
usbemu_manager_new ()
{
  return g_object_new (USBEMU_TYPE_MANAGER, NULL);
}

/**
 * usbemu_manager_get_devices:
 * @manager: A UsbemuManager instance.
 *
 * Returns: (element-type UsbemuDevice) (transfer full): A list of UsbemuDevice
 *          instances. The returned list must be freed by g_list_free() after
 *          each element has been freed with g_object_unref().
 */
GList*
usbemu_manager_get_devices (UsbemuManager *manager)
{
  UsbemuManagerPrivate *priv;

  g_return_val_if_fail (USBEMU_IS_MANAGER (manager), NULL);

  priv = USBEMU_MANAGER_GET_PRIVATE (manager);
  return g_list_copy_deep (priv->devices, (GCopyFunc) g_object_ref, NULL);
}
