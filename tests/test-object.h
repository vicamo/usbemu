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

#pragma once

#include <glib-object.h>

#include <usbemu/usbemu.h>

G_BEGIN_DECLS

#define USBEMU_TYPE_TEST_OBJECT  (usbemu_test_object_get_type ())

G_DECLARE_DERIVABLE_TYPE (UsbemuTestObject, usbemu_test_object,
                          USBEMU, TEST_OBJECT, GObject)

struct _UsbemuTestObjectClass {
  GObjectClass parent_class;
};

#define USBEMU_TEST_OBJECT_PROP_A_ENUM "a-enum"
#define USBEMU_TEST_OBJECT_PROP_A_FLAGS "a-flags"
#define USBEMU_TEST_OBJECT_PROP_A_STRING "a-string"
#define USBEMU_TEST_OBJECT_PROP_A_UINT "a-uint"

gint         usbemu_test_object_get_enum (UsbemuTestObject *tobject);
guint        usbemu_test_object_get_flags (UsbemuTestObject *tobject);
const gchar* usbemu_test_object_get_string (UsbemuTestObject *tobject);
guint        usbemu_test_object_get_uint (UsbemuTestObject *tobject);

G_END_DECLS
