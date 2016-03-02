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

#include <errno.h>
#include <stdlib.h>

#include <glib.h>
#include <gio/gio.h>

#include "usbemu-dbus-manager-object.h"

static gboolean opt_daemon = FALSE;
static gboolean opt_debug = FALSE;
static gchar * opt_name = "org.usbemu.UsbemuManager";
static gboolean opt_session = FALSE;
static gboolean opt_version = FALSE;

static GOptionEntry entries[] =
{
  { "daemon", 'D', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_debug, "Run as daemon", NULL },
  { "debug", 'd', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_debug, "Print debugging information", NULL },
  { "name", 'n', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING, &opt_name, "DBus name NAME to acquire. Default: \"org.usbemu.UsbemuManager\"", "NAME" },
  { "session", 's', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_session, "Bind to DBus session bus", NULL },
  { "version", 'v', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_version, "Show version", NULL },
  { NULL }
};

static void
on_name_acquired (GDBusConnection *connection,
                  const gchar     *name G_GNUC_UNUSED,
                  gpointer         user_data G_GNUC_UNUSED)
{
  g_info ("message bus name acquired");

  usbemu_dbus_manager_object_start (usbemu_dbus_manager_object_new (),
                                    connection);
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
  GMainLoop *loop;

  parse_arg (&argc, &argv);

  bus_type = (opt_session ? G_BUS_TYPE_SESSION : G_BUS_TYPE_SYSTEM);
  owner_id = g_bus_own_name (bus_type, opt_name,
                             G_BUS_NAME_OWNER_FLAGS_NONE,
                             NULL, on_name_acquired, NULL,
                             NULL, NULL);

  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  g_bus_unown_name (owner_id);
  g_main_loop_unref (loop);

  return 0;
}
