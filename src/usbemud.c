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

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <glib.h>
#include <gio/gio.h>

#include "usbemu/usbemu.h"
#include "usbemu-dbus-manager-object.h"
#include "usbemu-usbip-server.h"

static gboolean opt_daemon = FALSE;
static gboolean opt_ipv4 = FALSE;
static gboolean opt_ipv6 = FALSE;
static gint opt_tcp_port = 3240;
static gboolean opt_debug = FALSE;
static gchar * opt_name = USBEMU_DBUS_PREFIX;
static gboolean opt_session = FALSE;
static gboolean opt_version = FALSE;

static GOptionEntry entries[] =
{
  { "daemon", 'D', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_debug, "Run as daemon", NULL },
  { "debug", 'd', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_debug, "Print debugging information", NULL },
  { "ipv4", '4', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_ipv4, "Bind to IPv4. Default is both", NULL },
  { "ipv6", '6', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_ipv6, "Bind to IPv6. Default is both", NULL },
  { "name", 'n', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING, &opt_name,
    "DBus name NAME to acquire. Default: \"" USBEMU_DBUS_PREFIX "\"", "NAME" },
  { "session", 's', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_session, "Bind to DBus session bus", NULL },
  { "tcp-port", 't', 0, G_OPTION_ARG_INT, &opt_tcp_port, "Listen on TCP/IP port PORT", "PORT" },
  { "version", 'v', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_version, "Show version", NULL },
  { NULL }
};

static GMainLoop *loop;
static UsbemuDBusManagerObject *manager;
static GSocketService *sock_service;
static UsbemuUsbipServer *usbip_server;

static void
on_sock_event (GSocketListener     *listener G_GNUC_UNUSED,
               GSocketListenerEvent event,
               GSocket             *socket,
               gpointer             user_data G_GNUC_UNUSED)
{
  GEnumClass *event_class = g_type_class_ref (G_TYPE_SOCKET_LISTENER_EVENT);
  GEnumValue *event_value = g_enum_get_value (event_class, event);
  GSocketAddress *address;

  address = g_socket_get_local_address (socket, NULL);
  if (NULL != address) {
    GInetAddress *inet_address =
      g_inet_socket_address_get_address (G_INET_SOCKET_ADDRESS (address));
    gchar *str = g_inet_address_to_string (inet_address);

    g_debug ("sock_service %s event on %s", event_value->value_name, str);

    g_free (str);
    g_object_unref (address);
  } else {
    g_debug ("sock_service %s event", event_value->value_name);
  }

  g_type_class_unref (event_class);
}

static gboolean
on_sock_incoming (GSocketConnection *connection,
                  GObject           *source_object G_GNUC_UNUSED,
                  gpointer           user_data G_GNUC_UNUSED)
{
  if (NULL == usbip_server) {
    usbip_server = usbemu_usbip_server_new (manager);
    if (NULL == usbip_server) {
      g_warning ("Couldn't create usbip server.");
      return FALSE;
    }
  }

  usbemu_usbip_server_add_iostream (usbip_server, G_IO_STREAM (connection));
  return TRUE;
}

static void
start_sock_service ()
{
  GError * error = NULL;

  sock_service = g_socket_service_new ();

  if (opt_debug) {
    g_signal_connect (sock_service, "event", G_CALLBACK (on_sock_event), NULL);
  }

  if (opt_ipv4 && opt_ipv6) {
    g_socket_listener_add_inet_port ((GSocketListener *) sock_service,
                                     opt_tcp_port, NULL, &error);
  } else {
    GSocketFamily family = (opt_ipv4 ? G_SOCKET_FAMILY_IPV4 : G_SOCKET_FAMILY_IPV6);
    GInetAddress *inet_address = g_inet_address_new_any (family);

    GSocketAddress *sock_address = g_inet_socket_address_new (inet_address, opt_tcp_port);
    g_object_unref (inet_address);

    g_socket_listener_add_address ((GSocketListener *) sock_service,
                                   sock_address,
                                   G_SOCKET_TYPE_STREAM,
                                   G_SOCKET_PROTOCOL_TCP,
                                   NULL, NULL, &error);
    g_object_unref (sock_address);
  }

  if (error != NULL) {
    g_warning ("failed to open a listening socket: %s", error->message);
    g_error_free (error);

    g_object_unref (sock_service);
    sock_service = NULL;
    return;
  }

  g_signal_connect (sock_service, "incoming", G_CALLBACK (on_sock_incoming),
                    NULL);

  g_socket_service_start (sock_service);
}

static void
on_bus_acquired (GDBusConnection *connection G_GNUC_UNUSED,
                 const gchar     *name G_GNUC_UNUSED,
                 gpointer         user_data G_GNUC_UNUSED)
{
  g_info ("message bus acquired");

  manager = usbemu_dbus_manager_object_new ();
  if (manager == NULL) {
    g_warning ("Could not create manager");
    g_main_loop_quit (loop);
    return;
  }
}

static void
on_name_acquired (GDBusConnection *connection,
                  const gchar     *name G_GNUC_UNUSED,
                  gpointer         user_data G_GNUC_UNUSED)
{
  g_info ("message bus name '%s' acquired", opt_name);

  usbemu_dbus_manager_object_start (manager, connection);

  start_sock_service ();
}

static void
on_name_lost (GDBusConnection *connection G_GNUC_UNUSED,
              const gchar     *name G_GNUC_UNUSED,
              gpointer         user_data G_GNUC_UNUSED)
{
  g_info ("message bus name '%s' lost", opt_name);

  if (sock_service != NULL) {
    g_socket_service_stop (sock_service);
    g_object_unref (sock_service);
    sock_service = NULL;
  }

  g_object_unref (manager);
  manager = NULL;
}

static void
parse_arg (int   *argc,
           char **argv[])
{
  GOptionContext *context;
  GError *error = NULL;

  context = g_option_context_new ("- USB emulation service");
  g_option_context_add_main_entries (context, entries, NULL);
  g_option_context_parse (context, argc, argv, &error);
  g_option_context_free (context);

  if (error != NULL) {
    g_printerr ("option parsing failed: %s\n", error->message);
    g_error_free (error);
    exit (1);
  }

  if (opt_version) {
    g_print ("%s (%s)\n", g_get_prgname (), USBEMU_VERSION);
    exit (1);
  }

  if (opt_debug) {
    g_log_set_handler (G_LOG_DOMAIN, G_LOG_LEVEL_INFO,  g_log_default_handler, NULL);
    g_log_set_handler (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,  g_log_default_handler, NULL);
  }

  if (!opt_ipv4 && !opt_ipv6) {
    opt_ipv4 = opt_ipv6 = TRUE;
  }

  if (opt_tcp_port >= UINT16_MAX) {
    g_printerr ("port %u too high\n", opt_tcp_port);
    exit (1);
  }

  if (opt_daemon) {
    if (daemon (0, 0) < 0) {
      int saved_errno;

      saved_errno = errno;
      g_printerr ("Could not daemonize: %s [error %u]\n",
                  g_strerror (saved_errno), saved_errno);
      exit (1);
    }
  }
}

int
main (int   argc,
      char *argv[])
{
  GBusType bus_type;
  guint owner_id;

  parse_arg (&argc, &argv);

  g_info ("starting %s (%s)", g_get_prgname (), USBEMU_VERSION);

  bus_type = (opt_session ? G_BUS_TYPE_SESSION : G_BUS_TYPE_SYSTEM);
  owner_id = g_bus_own_name (bus_type, opt_name,
                             G_BUS_NAME_OWNER_FLAGS_NONE,
                             on_bus_acquired, on_name_acquired, on_name_lost,
                             NULL, NULL);

  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);
  g_main_loop_unref (loop);

  g_bus_unown_name (owner_id);

  g_info ("shutting down %s", g_get_prgname ());

  return 0;
}
