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
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>

#include "usbemu/usbemu.h"

static GMainLoop *main_loop = NULL;

static gboolean opt_background = FALSE;
static gboolean opt_debug = FALSE;
static gboolean opt_version = FALSE;
static gchar **opt_definitions = NULL;

static GOptionEntry main_entries[] =
{
  { "background", 'D', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_background,
    "Run in the background", NULL },
  { "debug", 'd', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_debug,
    "Print debugging information", NULL },
  { "version", 'v', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &opt_version,
    "Show version", NULL },
  { G_OPTION_REMAINING, '\0', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING_ARRAY, &opt_definitions,
    "Emulated device definition", NULL },
  { NULL }
};

static enum {
  ACTION_NONE,
  ACTION_ATTACH,
  ACTION_DETACH,
} opt_action = ACTION_NONE;
static gchar **opt_action_options = NULL;

static gboolean
parse_arg_attach (const gchar  *option_name,
                  const gchar  *value,
                  gpointer      data,
                   GError     **error)
{
  if (opt_action != ACTION_NONE) {
    g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED,
                 "conflicting actions for %s", option_name);
    return FALSE;
  }

  opt_action = (option_name[2] == 'a') ? ACTION_ATTACH : ACTION_DETACH;
  opt_action_options = g_strsplit ((value == NULL) ? "" : value, ",", 0);
  return TRUE;
}

static GOptionEntry action_entries[] =
{
  { "attach", '\0', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK,
    parse_arg_attach,
    "Attach device with comma-separated options", "<options>" },
  { "detach", '\0', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK,
    parse_arg_attach,
    "Detach device with comma-separated options", "<options>" },
  { NULL }
};

static void
parse_arg (int   *argc,
           char **argv[])
{
  GOptionContext *context;
  GOptionGroup *group;
  GError *error = NULL;

  context = g_option_context_new ("- USB emulation tool");
  g_option_context_add_main_entries (context, main_entries, NULL);
  g_option_context_set_help_enabled (context, TRUE);

  group = g_option_group_new ("actions", "Actions:", "Action options",
                              NULL, NULL);
  g_option_group_add_entries (group, action_entries);
  g_option_context_add_group (context, group);

  if (!g_option_context_parse (context, argc, argv, &error)) {
    if (error != NULL) {
      g_printerr ("option parsing failed: %s\n", error->message);
      g_error_free (error);
    } else
      g_printerr ("option parsing failed: unknown error\n");

    g_option_context_free (context);
    exit (1);
  }

  g_option_context_free (context);

  if (opt_version) {
    g_print ("%s (%s)\n", g_get_prgname (), USBEMU_VERSION);
    exit (0);
  }

  if (opt_debug) {
    g_log_set_handler (G_LOG_DOMAIN, G_LOG_LEVEL_INFO,  g_log_default_handler, NULL);
    g_log_set_handler (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,  g_log_default_handler, NULL);
  }

  if (opt_background) {
    if (daemon (0, 0) < 0) {
      int saved_errno;

      saved_errno = errno;
      g_printerr ("Could not daemonize: %s [error %u]\n",
                  g_strerror (saved_errno), saved_errno);
      exit (1);
    }
  }
}

static void
device_attach_callback (UsbemuDevice *device,
                        GAsyncResult *result,
                        gpointer      user_data G_GNUC_UNUSED)
{
  GError *error = NULL;

  if (!usbemu_device_attach_finish (device, result, &error)) {
    g_printerr ("Failed to attach device %04x:%04x, %s\n",
                usbemu_device_get_vendor_id (device),
                usbemu_device_get_product_id (device),
                error->message);
    g_main_loop_quit (main_loop);
    return;
  }

  g_print ("Device %04x:%04x attached.\n",
           usbemu_device_get_vendor_id (device),
           usbemu_device_get_product_id (device));

  /* TODO: */
  g_idle_add ((GSourceFunc) g_main_loop_quit, main_loop);
}

static void
device_detach_callback (UsbemuDevice *device,
                        GAsyncResult *result,
                        gpointer      user_data G_GNUC_UNUSED)
{
  GError *error = NULL;
  gboolean ret;

  ret = usbemu_device_detach_finish (device, result, &error);
  if (ret) {
    g_print ("Device %04x:%04x detached.\n",
             usbemu_device_get_vendor_id (device),
             usbemu_device_get_product_id (device));
  } else {
    g_printerr ("Failed to detach device %04x:%04x, %s\n",
                usbemu_device_get_vendor_id (device),
                usbemu_device_get_product_id (device),
                error->message);
  }

  g_idle_add ((GSourceFunc) g_main_loop_quit, main_loop);
}

int
main (int   argc,
      char *argv[])
{
  UsbemuDevice *device;
  GError *error = NULL;

  parse_arg (&argc, &argv);

  if (opt_action == ACTION_NONE) {
    g_print ("No action to take. Quit.\n");
    exit (0);
  }

  device = usbemu_device_new_from_argv (opt_definitions, &error);
  if (device == NULL) {
    if (error != NULL) {
      g_printerr ("failed to create device: %s\n", error->message);
      g_error_free (error);
    } else
      g_printerr ("failed to create device: unknown error.\n");

    exit (1);
  }

  g_print ("Created: %s, bcdUSB: 0x%04x, device %04x:%04x\n",
           g_type_name (G_OBJECT_TYPE (device)),
           usbemu_device_get_specification_num (device),
           usbemu_device_get_vendor_id (device),
           usbemu_device_get_product_id (device));

  main_loop = g_main_loop_new (NULL, FALSE);
  if (opt_action == ACTION_ATTACH) {
    usbemu_device_attach (device, opt_action_options, NULL,
                          (GAsyncReadyCallback) device_attach_callback, NULL);
  } else {
    usbemu_device_detach (device, opt_action_options, NULL,
                          (GAsyncReadyCallback) device_detach_callback, NULL);
  }

  g_main_loop_run (main_loop);

  while (g_main_context_pending (NULL)) {
    if (!g_main_context_iteration (NULL, FALSE))
      break;
  }

  g_object_unref (device);
  if (opt_action_options != NULL)
    g_strfreev (opt_action_options);
  if (opt_definitions != NULL)
    g_strfreev (opt_definitions);

  return 0;
}
