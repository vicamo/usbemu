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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "usbemu/usbemu.h"
#include "usbemu/dbus/usbemu-dbus-manager.h"

#include "usbemu/dbus/usbemu-dbus-object-manager.h"

/**
 * SECTION:UsbemuDBusObjectManagerClient
 * @title: UsbemuDBusObjectManagerClient
 * @short_description: Generated GDBusObjectManagerClient type
 *
 * This section contains a #GDBusObjectManagerClient that uses usbemu_dbus_object_manager_client_get_proxy_type() as the #GDBusProxyTypeFunc.
 */

/**
 * UsbemuDBusObjectManagerClient:
 *
 * The #UsbemuDBusObjectManagerClient structure contains only private data and should only be accessed using the provided API.
 */

/**
 * UsbemuDBusObjectManagerClientClass:
 * @parent_class: The parent class.
 *
 * Class structure for #UsbemuDBusObjectManagerClient.
 */

G_DEFINE_TYPE (UsbemuDBusObjectManagerClient, usbemu_dbus_object_manager_client, G_TYPE_DBUS_OBJECT_MANAGER_CLIENT);

static void
usbemu_dbus_object_manager_client_init (UsbemuDBusObjectManagerClient *manager G_GNUC_UNUSED)
{
}

static void
usbemu_dbus_object_manager_client_class_init (UsbemuDBusObjectManagerClientClass *klass G_GNUC_UNUSED)
{
}

/**
 * usbemu_dbus_object_manager_client_get_proxy_type:
 * @manager: A #GDBusObjectManagerClient.
 * @object_path: The object path of the remote object (unused).
 * @interface_name: (allow-none): Interface name of the remote object or %NULL to get the object proxy #GType.
 * @user_data: User data (unused).
 *
 * A #GDBusProxyTypeFunc that maps @interface_name to the generated #GDBusObjectProxy<!-- -->- and #GDBusProxy<!-- -->-derived types.
 *
 * Returns: A #GDBusProxy<!-- -->-derived #GType if @interface_name is not %NULL, otherwise the #GType for #UsbemuDBusObjectProxy.
 */
GType
usbemu_dbus_object_manager_client_get_proxy_type (GDBusObjectManagerClient *manager G_GNUC_UNUSED, const gchar *object_path G_GNUC_UNUSED, const gchar *interface_name, gpointer user_data G_GNUC_UNUSED)
{
  static gsize once_init_value = 0;
  static GHashTable *lookup_hash;
  GType ret;

  if (interface_name == NULL)
    return G_TYPE_DBUS_OBJECT_PROXY;
  if (g_once_init_enter (&once_init_value))
    {
      lookup_hash = g_hash_table_new (g_str_hash, g_str_equal);
      g_hash_table_insert (lookup_hash, (gpointer) USBEMU_DBUS_PREFIX, GSIZE_TO_POINTER (USBEMU_DBUS_TYPE_MANAGER_PROXY));
      g_once_init_leave (&once_init_value, 1);
    }
  ret = (GType) GPOINTER_TO_SIZE (g_hash_table_lookup (lookup_hash, interface_name));
  if (ret == (GType) 0)
    ret = G_TYPE_DBUS_PROXY;
  return ret;
}

/**
 * usbemu_dbus_object_manager_client_new:
 * @connection: A #GDBusConnection.
 * @flags: Flags from the #GDBusObjectManagerClientFlags enumeration.
 * @name: (allow-none): A bus name (well-known or unique) or %NULL if @connection is not a message bus connection.
 * @object_path: An object path.
 * @cancellable: (allow-none): A #GCancellable or %NULL.
 * @callback: A #GAsyncReadyCallback to call when the request is satisfied.
 * @user_data: User data to pass to @callback.
 *
 * Asynchronously creates #GDBusObjectManagerClient using usbemu_dbus_object_manager_client_get_proxy_type() as the #GDBusProxyTypeFunc. See g_dbus_object_manager_client_new() for more details.
 *
 * When the operation is finished, @callback will be invoked in the <link linkend="g-main-context-push-thread-default">thread-default main loop</link> of the thread you are calling this method from.
 * You can then call usbemu_dbus_object_manager_client_new_finish() to get the result of the operation.
 *
 * See usbemu_dbus_object_manager_client_new_sync() for the synchronous, blocking version of this constructor.
 */
void
usbemu_dbus_object_manager_client_new (
    GDBusConnection        *connection,
    GDBusObjectManagerClientFlags  flags,
    const gchar            *name,
    const gchar            *object_path,
    GCancellable           *cancellable,
    GAsyncReadyCallback     callback,
    gpointer                user_data)
{
  g_async_initable_new_async (USBEMU_DBUS_TYPE_OBJECT_MANAGER_CLIENT, G_PRIORITY_DEFAULT, cancellable, callback, user_data, "flags", flags, "name", name, "connection", connection, "object-path", object_path, "get-proxy-type-func", usbemu_dbus_object_manager_client_get_proxy_type, NULL);
}

/**
 * usbemu_dbus_object_manager_client_new_finish:
 * @res: The #GAsyncResult obtained from the #GAsyncReadyCallback passed to usbemu_dbus_object_manager_client_new().
 * @error: Return location for error or %NULL
 *
 * Finishes an operation started with usbemu_dbus_object_manager_client_new().
 *
 * Returns: (transfer full) (type UsbemuDBusObjectManagerClient): The constructed object manager client or %NULL if @error is set.
 */
GDBusObjectManager *
usbemu_dbus_object_manager_client_new_finish (
    GAsyncResult        *res,
    GError             **error)
{
  GObject *ret;
  GObject *source_object;
  source_object = g_async_result_get_source_object (res);
  ret = g_async_initable_new_finish (G_ASYNC_INITABLE (source_object), res, error);
  g_object_unref (source_object);
  if (ret != NULL)
    return G_DBUS_OBJECT_MANAGER (ret);
  else
    return NULL;
}

/**
 * usbemu_dbus_object_manager_client_new_sync:
 * @connection: A #GDBusConnection.
 * @flags: Flags from the #GDBusObjectManagerClientFlags enumeration.
 * @name: (allow-none): A bus name (well-known or unique) or %NULL if @connection is not a message bus connection.
 * @object_path: An object path.
 * @cancellable: (allow-none): A #GCancellable or %NULL.
 * @error: Return location for error or %NULL
 *
 * Synchronously creates #GDBusObjectManagerClient using usbemu_dbus_object_manager_client_get_proxy_type() as the #GDBusProxyTypeFunc. See g_dbus_object_manager_client_new_sync() for more details.
 *
 * The calling thread is blocked until a reply is received.
 *
 * See usbemu_dbus_object_manager_client_new() for the asynchronous version of this constructor.
 *
 * Returns: (transfer full) (type UsbemuDBusObjectManagerClient): The constructed object manager client or %NULL if @error is set.
 */
GDBusObjectManager *
usbemu_dbus_object_manager_client_new_sync (
    GDBusConnection        *connection,
    GDBusObjectManagerClientFlags  flags,
    const gchar            *name,
    const gchar            *object_path,
    GCancellable           *cancellable,
    GError                **error)
{
  GInitable *ret;
  ret = g_initable_new (USBEMU_DBUS_TYPE_OBJECT_MANAGER_CLIENT, cancellable, error, "flags", flags, "name", name, "connection", connection, "object-path", object_path, "get-proxy-type-func", usbemu_dbus_object_manager_client_get_proxy_type, NULL);
  if (ret != NULL)
    return G_DBUS_OBJECT_MANAGER (ret);
  else
    return NULL;
}


/**
 * usbemu_dbus_object_manager_client_new_for_bus:
 * @bus_type: A #GBusType.
 * @flags: Flags from the #GDBusObjectManagerClientFlags enumeration.
 * @name: A bus name (well-known or unique).
 * @object_path: An object path.
 * @cancellable: (allow-none): A #GCancellable or %NULL.
 * @callback: A #GAsyncReadyCallback to call when the request is satisfied.
 * @user_data: User data to pass to @callback.
 *
 * Like usbemu_dbus_object_manager_client_new() but takes a #GBusType instead of a #GDBusConnection.
 *
 * When the operation is finished, @callback will be invoked in the <link linkend="g-main-context-push-thread-default">thread-default main loop</link> of the thread you are calling this method from.
 * You can then call usbemu_dbus_object_manager_client_new_for_bus_finish() to get the result of the operation.
 *
 * See usbemu_dbus_object_manager_client_new_for_bus_sync() for the synchronous, blocking version of this constructor.
 */
void
usbemu_dbus_object_manager_client_new_for_bus (
    GBusType                bus_type,
    GDBusObjectManagerClientFlags  flags,
    const gchar            *name,
    const gchar            *object_path,
    GCancellable           *cancellable,
    GAsyncReadyCallback     callback,
    gpointer                user_data)
{
  g_async_initable_new_async (USBEMU_DBUS_TYPE_OBJECT_MANAGER_CLIENT, G_PRIORITY_DEFAULT, cancellable, callback, user_data, "flags", flags, "name", name, "bus-type", bus_type, "object-path", object_path, "get-proxy-type-func", usbemu_dbus_object_manager_client_get_proxy_type, NULL);
}

/**
 * usbemu_dbus_object_manager_client_new_for_bus_finish:
 * @res: The #GAsyncResult obtained from the #GAsyncReadyCallback passed to usbemu_dbus_object_manager_client_new_for_bus().
 * @error: Return location for error or %NULL
 *
 * Finishes an operation started with usbemu_dbus_object_manager_client_new_for_bus().
 *
 * Returns: (transfer full) (type UsbemuDBusObjectManagerClient): The constructed object manager client or %NULL if @error is set.
 */
GDBusObjectManager *
usbemu_dbus_object_manager_client_new_for_bus_finish (
    GAsyncResult        *res,
    GError             **error)
{
  GObject *ret;
  GObject *source_object;
  source_object = g_async_result_get_source_object (res);
  ret = g_async_initable_new_finish (G_ASYNC_INITABLE (source_object), res, error);
  g_object_unref (source_object);
  if (ret != NULL)
    return G_DBUS_OBJECT_MANAGER (ret);
  else
    return NULL;
}

/**
 * usbemu_dbus_object_manager_client_new_for_bus_sync:
 * @bus_type: A #GBusType.
 * @flags: Flags from the #GDBusObjectManagerClientFlags enumeration.
 * @name: A bus name (well-known or unique).
 * @object_path: An object path.
 * @cancellable: (allow-none): A #GCancellable or %NULL.
 * @error: Return location for error or %NULL
 *
 * Like usbemu_dbus_object_manager_client_new_sync() but takes a #GBusType instead of a #GDBusConnection.
 *
 * The calling thread is blocked until a reply is received.
 *
 * See usbemu_dbus_object_manager_client_new_for_bus() for the asynchronous version of this constructor.
 *
 * Returns: (transfer full) (type UsbemuDBusObjectManagerClient): The constructed object manager client or %NULL if @error is set.
 */
GDBusObjectManager *
usbemu_dbus_object_manager_client_new_for_bus_sync (
    GBusType                bus_type,
    GDBusObjectManagerClientFlags  flags,
    const gchar            *name,
    const gchar            *object_path,
    GCancellable           *cancellable,
    GError                **error)
{
  GInitable *ret;
  ret = g_initable_new (USBEMU_DBUS_TYPE_OBJECT_MANAGER_CLIENT, cancellable, error, "flags", flags, "name", name, "bus-type", bus_type, "object-path", object_path, "get-proxy-type-func", usbemu_dbus_object_manager_client_get_proxy_type, NULL);
  if (ret != NULL)
    return G_DBUS_OBJECT_MANAGER (ret);
  else
    return NULL;
}
