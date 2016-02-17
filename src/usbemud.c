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

#include <glib.h>

static gboolean opt_debug = FALSE;
static gboolean opt_version = FALSE;

static GOptionEntry entries[] =
{
  { "debug", 'd', 0, G_OPTION_ARG_NONE, &opt_debug, "Print debugging information", NULL },
  { "version", 'v', 0, G_OPTION_ARG_NONE, &opt_version, "Show version", NULL },
  { NULL }
};

int
main (int argc, char** argv)
{
  GOptionContext *context;
  GError *error = NULL;

  context = g_option_context_new ("- USB emulation service");
  g_option_context_add_main_entries (context, entries, NULL);
  g_option_context_parse (context, &argc, &argv, &error);
  g_option_context_free (context);

  if (error != NULL) {
    g_printerr ("option parsing failed: %s\n", error->message);
    g_error_free (error);
    return 1;
  }

  if (opt_version) {
    g_print ("%s (%s)\n", g_get_prgname (), USBEMU_VERSION);
    return 0;
  }

  if (opt_debug) {
    g_log_set_handler (G_LOG_DOMAIN, G_LOG_LEVEL_INFO,  g_log_default_handler, NULL);
    g_log_set_handler (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,  g_log_default_handler, NULL);
  }

  return 0;
}
