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

#include <stdlib.h>

#include <glib.h>
#include <gio/gio.h>

#include "usbemu/usbemu.h"

typedef void (*UsbipCmdFunc) (int argc, char *argv[]);

typedef struct {
  const gchar *name;
  const GOptionEntry *entries;
  UsbipCmdFunc func;
  const gchar *description;
  const gchar *help_description;
} UsbipCmdEntry;

static gboolean opt_debug = FALSE;
static gboolean opt_log = FALSE;
extern gchar * opt_name;
extern gboolean opt_session;
static gchar **opt_commands;

static const GOptionEntry main_entries[] =
{
  { "debug", 'd', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_debug,
    "Print debugging information", NULL },
  { "log", 'l', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_log,
    "Log to syslog", NULL },
  { "name", 'n', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING, &opt_name,
    "DBus name NAME to connect. Default: \"" USBEMU_DBUS_PREFIX "\"", "NAME" },
  { "session", 's', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_session,
    "Connect to DBus session bus", NULL },
  { G_OPTION_REMAINING, 0, G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_STRING_ARRAY,
    &opt_commands, NULL, NULL },
  { NULL }
};

static gchar **opt_help_command;

static const GOptionEntry help_entries[] =
{
  { G_OPTION_REMAINING, 0, G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_STRING_ARRAY,
    &opt_help_command, NULL, NULL },
  { NULL }
};

static void do_help    (int   argc,
                        char *argv[]);
static void do_version (int   argc,
                        char *argv[]);
extern void do_list    (int   argc,
                        char *argv[]);

extern const GOptionEntry list_entries[];

static const UsbipCmdEntry commands[] =
{
  { "help", help_entries, do_help, "Command 'help' Options:", "" },
  { "version", NULL, do_version, "Command 'version' Options:", "" },
  { "list", list_entries, do_list, "Command 'list' Options:", "" },
  { NULL }
};

static GOptionGroup*
add_help_group (GOptionContext      *context,
                const UsbipCmdEntry *command)
{
  GOptionGroup *group;

  group = g_option_group_new (command->name,
                              command->description,
                              command->help_description,
                              NULL, NULL);
  if (command->entries != NULL) {
    g_option_group_add_entries (group, command->entries);
  }

  g_option_context_add_group (context, group);
  return group;
}

static void
do_help (int   argc G_GNUC_UNUSED,
         char *argv[] G_GNUC_UNUSED)
{
  GOptionContext *context;
  const UsbipCmdEntry *command;
  gchar *help = NULL;

  context = g_option_context_new ("[help] <command> [COMMAND_OPTION...]");
  g_option_context_set_help_enabled (context, FALSE);
  g_option_context_add_main_entries (context, main_entries, NULL);

  if ((opt_help_command != NULL) &&
      (g_strv_length (opt_help_command) == 1)) {
    for (command = &commands[0]; command->name != NULL; command++) {
      if (!g_str_equal (opt_help_command[0], command->name)) {
        continue;
      }

      help = g_option_context_get_help (context, FALSE,
                                        add_help_group (context, command));
      break;
    }
  }

  if (help == NULL) {
    for (command = &commands[0]; command->name != NULL; command++) {
      add_help_group (context, command);
    }

    help = g_option_context_get_help (context, FALSE, NULL);
  }

  g_option_context_free (context);

  g_print (help);
  g_free (help);
}

static void
do_version (int   argc G_GNUC_UNUSED,
            char *argv[] G_GNUC_UNUSED)
{
  g_print ("%s (%s)\n", g_get_prgname (), USBEMU_VERSION);
}

static void
do_command (const UsbipCmdEntry *command,
            int                  argc,
            char                *argv[])
{
  if (command->entries != NULL) {
    GOptionContext *context;
    GError *error = NULL;

    context = g_option_context_new (NULL);

    if (command->description != NULL) {
      g_option_context_set_description (context, command->description);
    }
    g_option_context_add_main_entries (context, command->entries, NULL);
    g_option_context_parse (context, &argc, &argv, &error);

    g_option_context_free (context);

    if (error != NULL) {
      g_printerr ("option parsing failed: %s\n", error->message);
      g_error_free (error);
      exit (1);
    }
  }

  (command->func) (argc, argv);
}

static void
parse_arg (int   *argc,
           char **argv[])
{
  GOptionContext *context;
  GError *error = NULL;

  context = g_option_context_new (NULL);
  g_option_context_set_help_enabled (context, FALSE);
  g_option_context_set_strict_posix (context, TRUE);
  g_option_context_add_main_entries (context, main_entries, NULL);
  g_option_context_parse (context, argc, argv, &error);
  g_option_context_free (context);

  if (error != NULL) {
    g_printerr ("option parsing failed: %s\n", error->message);
    g_error_free (error);
    exit (1);
  }

  if (!g_dbus_is_name (opt_name)) {
    g_printerr ("invalid bus name: %s\n", opt_name);
    exit (1);
  }
}

int
main (int   argc,
      char *argv[])
{
  parse_arg (&argc, &argv);

  if (opt_commands) {
    const UsbipCmdEntry *command;

    for (command = &commands[0]; command->name != NULL; ++command) {
      if (g_str_equal (command->name, opt_commands[0])) {
        do_command (command, g_strv_length (opt_commands), opt_commands);
        exit (0);
      }
    }
  }

  do_help (0, NULL);

  return 0;
}