/* usbemu - USB Emulation Library
 * Copyright (C) 2018 You-Sheng Yang
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

#if defined (HAVE_CONFIG_H)
#include "config.h"
#endif

#include <locale.h>
#include <glib.h>

#include "usbemu/usbemu.h"

static void
test_basic_1 (void)
{
  g_assert_true (G_TYPE_IS_ENUM (USBEMU_TYPE_CLASSES));
  g_assert_true (G_TYPE_IS_ENUM (USBEMU_TYPE_ENDPOINT_DIRECTIONS));
  g_assert_true (G_TYPE_IS_ENUM (USBEMU_TYPE_ENDPOINT_ISOCHRONOUS_SYNCS));
  g_assert_true (G_TYPE_IS_ENUM (USBEMU_TYPE_ENDPOINT_ISOCHRONOUS_USAGES));
  g_assert_true (G_TYPE_IS_ENUM (USBEMU_TYPE_ENDPOINT_TRANSFERS));
  g_assert_true (G_TYPE_IS_ENUM (USBEMU_TYPE_ENDPOINTS));
  g_assert_true (G_TYPE_IS_ENUM (USBEMU_TYPE_ERROR));

  g_assert_true (G_TYPE_IS_FLAGS (USBEMU_TYPE_CONFIGURATION_ATTRIBUTES));
}

int
main (int   argc,
      char *argv[])
{
  setlocale (LC_ALL, "");

  g_test_init (&argc, &argv, NULL);
  g_test_bug_base (PACKAGE_BUGREPORT);

  g_test_add_func ("/UsbemuEnums/basic", test_basic_1);

  return g_test_run ();
}
