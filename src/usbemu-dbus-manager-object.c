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

#include "config.h"

#include <usbemu/usbemu.h>
#include <usbemu/dbus/usbemu-dbus-manager.h>

#include "usbemu-dbus-device-object.h"
#include "usbemu-dbus-manager-object.h"

typedef struct  _UsbemuDBusManagerObjectPrivate {
  GDBusObjectManagerServer *object_manager;
  GHashTable *device_path_hash;
  guint device_path_counter;

  UsbemuDBusManagerSkeleton *manager_skeleton;
} UsbemuDBusManagerObjectPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (UsbemuDBusManagerObject, usbemu_dbus_manager_object,
                            G_TYPE_DBUS_OBJECT_SKELETON)

#define USBEMU_DBUS_MANAGER_OBJECT_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), USBEMU_TYPE_DBUS_MANAGER_OBJECT, \
                                UsbemuDBusManagerObjectPrivate))

static void
update_prop_devices (UsbemuDBusManager *manager_iface,
                     GHashTable        *device_path_hash,
                     const gchar       *path,
                     gboolean           is_added)
{
  const gchar **paths;
  guint length = 0;

  paths =
    (const gchar **) g_hash_table_get_keys_as_array (device_path_hash, &length);
  usbemu_dbus_manager_set_devices (manager_iface, paths);
  g_free (paths);

  if (is_added) {
    usbemu_dbus_manager_emit_device_added (manager_iface, path);
  } else {
    usbemu_dbus_manager_emit_device_removed (manager_iface, path);
  }
}

static gboolean
remove_device (UsbemuDBusManagerObjectPrivate *priv,
               UsbemuDBusManager              *manager_iface,
               const gchar                    *path)
{
  GDBusObjectSkeleton *device;

  device = G_DBUS_OBJECT_SKELETON (g_hash_table_lookup (priv->device_path_hash,
                                                        path));
  if (device == NULL) {
    return FALSE;
  }

  g_assert (g_dbus_object_manager_server_is_exported (priv->object_manager,
                                                      device));
  if (!g_dbus_object_manager_server_unexport (priv->object_manager, path)) {
    g_assert_not_reached ();
  }

  g_hash_table_remove (priv->device_path_hash, path);
  update_prop_devices (manager_iface, priv->device_path_hash, path, FALSE);

  return TRUE;
}

static void
remove_all_devices (UsbemuDBusManagerObjectPrivate *priv)
{
  UsbemuDBusManager *manager_iface;
  GHashTableIter iter;
  const gchar *path;

  manager_iface = USBEMU_DBUS_MANAGER (priv->manager_skeleton);

  g_hash_table_iter_init (&iter, priv->device_path_hash);
  while (g_hash_table_iter_next (&iter, (gpointer) &path, NULL)) {
    remove_device (priv, manager_iface, path);

    g_hash_table_iter_init (&iter, priv->device_path_hash);
  }
}

static gboolean
on_handle_add_device (UsbemuDBusManager       *manager_iface,
                      GDBusMethodInvocation   *invocation,
                      GVariant                *settings G_GNUC_UNUSED,
                      UsbemuDBusManagerObject *manager)
{
  UsbemuDBusManagerObjectPrivate *priv;
  UsbemuDevice *device;
  UsbemuDBusDeviceObject *device_obj;
  guint id;
  gchar *path;

  priv = USBEMU_DBUS_MANAGER_OBJECT_GET_PRIVATE (manager);

  device = usbemu_device_new ("dummy");
  if (device == NULL) {
    g_dbus_method_invocation_return_error_literal (invocation,
                                                   USBEMU_MANAGER_ERROR,
                                                   USBEMU_MANAGER_ERROR_FAILED,
                                                   "add device failed");
    return TRUE;
  }

  id = ++(priv->device_path_counter);
  path = g_strdup_printf (USBEMU_DBUS_DEVICE_PATH "/%u", id);

  device_obj = usbemu_dbus_device_object_new (path, device);
  g_dbus_object_manager_server_export (priv->object_manager,
                                       G_DBUS_OBJECT_SKELETON (device_obj));

  g_hash_table_insert (priv->device_path_hash, path, g_object_ref (device_obj));
  update_prop_devices (manager_iface, priv->device_path_hash, path, TRUE);

  usbemu_dbus_manager_complete_add_device (manager_iface, invocation, path);

  return TRUE;
}

static gboolean
on_handle_remove_device (UsbemuDBusManager       *manager_iface,
                         GDBusMethodInvocation   *invocation,
                         const gchar             *path,
                         UsbemuDBusManagerObject *manager)
{
  UsbemuDBusManagerObjectPrivate *priv =
    USBEMU_DBUS_MANAGER_OBJECT_GET_PRIVATE (manager);

  if (!remove_device (priv, manager_iface, path)) {
    g_dbus_method_invocation_return_error (invocation,
                                           USBEMU_MANAGER_ERROR,
                                           USBEMU_MANAGER_ERROR_DEVICE_UNAVAILABLE,
                                           "device '%s' not found",
                                           path);
  } else {
    usbemu_dbus_manager_complete_remove_device (manager_iface, invocation);
  }

  return TRUE;
}

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

  g_signal_connect (manager_skeleton, "handle-add-device",
                    G_CALLBACK (on_handle_add_device), manager);
  g_signal_connect (manager_skeleton, "handle-remove-device",
                    G_CALLBACK (on_handle_remove_device), manager);

  g_dbus_object_skeleton_add_interface (G_DBUS_OBJECT_SKELETON (manager),
                                        G_DBUS_INTERFACE_SKELETON (manager_skeleton));
}

static void
dispose (GObject *object)
{
  UsbemuDBusManagerObject *manager = USBEMU_DBUS_MANAGER_OBJECT (object);
  UsbemuDBusManagerObjectPrivate *priv =
    USBEMU_DBUS_MANAGER_OBJECT_GET_PRIVATE (manager);

  remove_all_devices (priv);

  g_dbus_object_skeleton_remove_interface (G_DBUS_OBJECT_SKELETON (object),
                                           G_DBUS_INTERFACE_SKELETON (priv->manager_skeleton));
  g_object_unref (priv->manager_skeleton);
  priv->manager_skeleton = NULL;
}

static void
finalize (GObject *object)
{
  UsbemuDBusManagerObject *manager = USBEMU_DBUS_MANAGER_OBJECT (object);
  UsbemuDBusManagerObjectPrivate *priv =
    USBEMU_DBUS_MANAGER_OBJECT_GET_PRIVATE (manager);

  g_assert (g_hash_table_size (priv->device_path_hash) == 0);
  g_hash_table_destroy (priv->device_path_hash);
}

static void
usbemu_dbus_manager_object_class_init (UsbemuDBusManagerObjectClass *manager_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (manager_class);

  /* virtual methods */

  object_class->constructed = constructed;
  object_class->dispose = dispose;
  object_class->finalize = finalize;
}

static void
usbemu_dbus_manager_object_init (UsbemuDBusManagerObject *manager)
{
  UsbemuDBusManagerObjectPrivate *priv =
    USBEMU_DBUS_MANAGER_OBJECT_GET_PRIVATE (manager);

  priv->device_path_hash = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                  g_free, g_object_unref);
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