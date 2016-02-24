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

#include <glib.h>

#include "usbemu-usbip-server.h"

/**
 * UsbemuUsbipServer:
 *
 * An object representing emulated USB usbip server.
 */

/**
 * UsbemuUsbipServerClass:
 * @parent_class: The parent class.
 *
 * Class structure for UsbemuUsbipServer.
 */

typedef struct  _UsbemuUsbipServerPrivate {
  UsbemuDBusManagerObject *manager;
  GHashTable *pool;
} UsbemuUsbipServerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (UsbemuUsbipServer, usbemu_usbip_server, \
                            G_TYPE_OBJECT)

#define USBEMU_USBIP_SERVER_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), USBEMU_TYPE_USBIP_SERVER, \
                                UsbemuUsbipServerPrivate))

enum
{
  PROP_0,
  PROP_MANAGER,
  N_PROPERTIES
};

static GParamSpec *props[N_PROPERTIES] = { NULL, };

static void
remove_connection (UsbemuUsbipServerPrivate *priv,
                   GIOStream                *iostream)
{
  g_hash_table_remove (priv->pool, iostream);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
  UsbemuUsbipServer *server = USBEMU_USBIP_SERVER (object);
  UsbemuUsbipServerPrivate *priv = USBEMU_USBIP_SERVER_GET_PRIVATE (server);

  switch (prop_id) {
    case PROP_MANAGER:
      g_assert (priv->manager == NULL);
      priv->manager = g_value_dup_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
  UsbemuUsbipServer *server = USBEMU_USBIP_SERVER (object);

  switch (prop_id) {
    case PROP_MANAGER:
      g_value_set_object (value, usbemu_usbip_server_get_manager (server));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
dispose (GObject *object)
{
  UsbemuUsbipServer *server = USBEMU_USBIP_SERVER (object);
  UsbemuUsbipServerPrivate *priv = USBEMU_USBIP_SERVER_GET_PRIVATE (server);
  GHashTableIter iter;
  gpointer key = NULL;

  g_hash_table_iter_init (&iter, priv->pool);
  while (g_hash_table_iter_next (&iter, &key, NULL)) {
    remove_connection (priv, G_IO_STREAM (key));
  }

  g_object_unref (priv->manager);
  priv->manager = NULL;
}

static void
finalize (GObject *object)
{
  UsbemuUsbipServer *server = USBEMU_USBIP_SERVER (object);
  UsbemuUsbipServerPrivate *priv = USBEMU_USBIP_SERVER_GET_PRIVATE (server);

  g_assert (g_hash_table_size (priv->pool) == 0);
  g_hash_table_unref (priv->pool);
  priv->pool = NULL;
}

static void
usbemu_usbip_server_class_init (UsbemuUsbipServerClass *server_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (server_class);

  /* virtual methods */

  object_class->set_property = set_property;
  object_class->get_property = get_property;
  object_class->dispose = dispose;
  object_class->finalize = finalize;

  /* properties */

  props[PROP_MANAGER] =
    g_param_spec_object (USBEMU_USBIP_SERVER_PROP_MANAGER,
                         "Manager", "Manager",
                         USBEMU_TYPE_DBUS_MANAGER_OBJECT,
                         G_PARAM_READWRITE | \
                           G_PARAM_CONSTRUCT_ONLY | \
                           G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPERTIES, props);
}

static void
usbemu_usbip_server_init (UsbemuUsbipServer *server)
{
  UsbemuUsbipServerPrivate *priv = USBEMU_USBIP_SERVER_GET_PRIVATE (server);
  GHashTable *pool;

  pool = g_hash_table_new_full (g_direct_hash, g_direct_equal,
                                NULL, g_object_unref);
  priv->pool = g_hash_table_ref (pool);
}

/**
 * usbemu_usbip_server_new:
 * @manager: A USBEmu DBus manager server-side object.
 *
 * Returns: (transfer full) (type UsbemuUsbipServer): The constructed server
 *          object or %NULL.
 */
UsbemuUsbipServer*
usbemu_usbip_server_new (UsbemuDBusManagerObject *manager)
{
  g_return_val_if_fail (USBEMU_IS_DBUS_MANAGER_OBJECT (manager), NULL);

  return g_object_new (USBEMU_TYPE_USBIP_SERVER,
                       USBEMU_USBIP_SERVER_PROP_MANAGER, manager,
                       NULL);
}

/**
 * usbemu_usbip_server_get_manager:
 * @server: A valid UsbemuUsbipServer instance.
 *
 * Returns: (transfer none) (type UsbemuDBusManagerObject): The underlying
 *          manager object for this server or %NULL.
 */
UsbemuDBusManagerObject*
usbemu_usbip_server_get_manager (UsbemuUsbipServer *server)
{
  g_return_val_if_fail (USBEMU_IS_USBIP_SERVER (server), NULL);

  return USBEMU_USBIP_SERVER_GET_PRIVATE (server)->manager;
}

/**
 * usbemu_usbip_server_get_manager:
 * @server: A valid UsbemuUsbipServer instance.
 *
 * Returns: (transfer full) (type UsbemuDBusManagerObject): The underlying
 *          manager object for this server or %NULL. should be unreferenced when
 *          no longer needed.
 */
UsbemuDBusManagerObject*
usbemu_usbip_server_dup_manager (UsbemuUsbipServer *server)
{
  return g_object_ref (usbemu_usbip_server_get_manager (server));
}

/**
 * usbemu_usbip_server_add_iostream:
 * @server: A valid UsbemuUsbipServer instance.
 * @iostream:
 *
 * Add a connection to usbip server's processing pool.
 */
void
usbemu_usbip_server_add_iostream (UsbemuUsbipServer *server,
                                  GIOStream         *iostream)
{
  UsbemuUsbipServerPrivate *priv;

  g_return_if_fail (USBEMU_IS_USBIP_SERVER (server));
  g_return_if_fail (G_IS_IO_STREAM (iostream));

  priv = USBEMU_USBIP_SERVER_GET_PRIVATE (server);
  if (g_hash_table_contains (priv->pool, iostream)) {
    return;
  }

  g_hash_table_add (priv->pool, g_object_ref (iostream));
}