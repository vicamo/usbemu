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
test_instanciation_new_1 (void)
{
  UsbemuInterface *interface;

  interface = usbemu_interface_new ();
  g_assert_nonnull (interface);
  g_assert_true (USBEMU_IS_INTERFACE (interface));

  g_object_unref (interface);
}

static void
test_instanciation_new_full_1 (void)
{
  const struct {
    const gchar *name;
    UsbemuClasses klass;
    guint sub_class;
    guint protocol;
  } entries[] = {
    { NULL,
      USBEMU_CLASS_USE_INTERFACE_DESCRIPTOR,
      USBEMU_SUB_CLASS_USE_INTERFACE_DESCRIPTOR,
      USBEMU_PROTOCOL_USE_INTERFACE_DESCRIPTOR, },
    { "name 1", USBEMU_CLASS_AUDIO, 0x01, 0x01 },
    { NULL,
      USBEMU_CLASS_USE_INTERFACE_DESCRIPTOR,
      USBEMU_SUB_CLASS_USE_INTERFACE_DESCRIPTOR,
      USBEMU_PROTOCOL_USE_INTERFACE_DESCRIPTOR, },
  };
  UsbemuInterface *interface;
  gsize i;

  for (i = 0; i < G_N_ELEMENTS (entries); i++) {
    interface =
        usbemu_interface_new_full (entries[i].name, entries[i].klass,
                                   entries[i].sub_class, entries[i].protocol);
    g_assert_nonnull (interface);
    g_assert_true (USBEMU_IS_INTERFACE (interface));

    if (entries[i].name == NULL)
      g_assert_null (usbemu_interface_get_name (interface));
    else
      g_assert_cmpstr (usbemu_interface_get_name (interface), ==,
                       entries[i].name);
    g_assert_cmpuint (usbemu_interface_get_class (interface), ==,
                      entries[i].klass);
    g_assert_cmpuint (usbemu_interface_get_sub_class (interface), ==,
                      entries[i].sub_class);
    g_assert_cmpuint (usbemu_interface_get_protocol (interface), ==,
                      entries[i].protocol);

    g_object_unref (interface);
  }
}

static void
test_properties_name_1 (void)
{
  const gchar *names[] = {
    "1", NULL,
  };
  UsbemuInterface *interface;
  gsize i;
  GValue value = G_VALUE_INIT;

  interface = usbemu_interface_new ();
  g_test_queue_unref (interface);

  /* default name is NULL. */
  g_assert_null (usbemu_interface_get_name (interface));

  for (i = 0; i < G_N_ELEMENTS (names); i++) {
    g_value_init (&value, G_TYPE_STRING);

    usbemu_interface_set_name (interface, names[i]);
    g_object_get_property ((GObject*) interface,
                           USBEMU_INTERFACE_PROP_NAME, &value);
    if (names[i] == NULL) {
      g_assert_null (usbemu_interface_get_name (interface));
      g_assert_null (g_value_get_string (&value));
    } else {
      g_assert_cmpstr (usbemu_interface_get_name (interface), ==,
                       names[i]);
      g_assert_cmpstr (g_value_get_string (&value), ==, names[i]);
    }

    g_value_unset (&value);
  }
}

static void
test_properties_class_1 (void)
{
  const UsbemuClasses classes[] = {
    USBEMU_CLASS_USE_INTERFACE_DESCRIPTOR,
    USBEMU_CLASS_VENDOR_SPECIFIC,
  };
  UsbemuInterface *interface;
  gsize i;
  GValue value = G_VALUE_INIT;

  interface = usbemu_interface_new ();
  g_test_queue_unref (interface);

  /* default class is USBEMU_CLASS_VENDOR_SPECIFIC. */
  g_assert_cmpuint (usbemu_interface_get_class (interface), ==,
                    USBEMU_CLASS_VENDOR_SPECIFIC);

  for (i = 0; i < G_N_ELEMENTS (classes); i++) {
    usbemu_interface_set_class (interface, classes[i]);
    g_assert_cmpuint (usbemu_interface_get_class (interface), ==, classes[i]);

    g_value_init (&value, USBEMU_TYPE_CLASSES);
    g_object_get_property ((GObject*) interface,
                           USBEMU_INTERFACE_PROP_CLASS, &value);
    g_assert_cmpuint (g_value_get_enum (&value), ==, classes[i]);
    g_value_unset (&value);
  }
}

static void
test_properties_sub_class_1 (void)
{
  const guint sub_classes[] = {
    0x01, 0x02, 0xFF, USBEMU_SUB_CLASS_USE_INTERFACE_DESCRIPTOR,
  };
  UsbemuInterface *interface;
  gsize i;
  GValue value = G_VALUE_INIT;

  interface = usbemu_interface_new ();
  g_test_queue_unref (interface);

  /* default class is 0. */
  g_assert_cmpuint (usbemu_interface_get_sub_class (interface), ==, 0);

  for (i = 0; i < G_N_ELEMENTS (sub_classes); i++) {
    usbemu_interface_set_sub_class (interface, sub_classes[i]);
    g_assert_cmpuint (usbemu_interface_get_sub_class (interface), ==, sub_classes[i]);

    g_value_init (&value, G_TYPE_UINT);
    g_object_get_property ((GObject*) interface,
                           USBEMU_INTERFACE_PROP_SUB_CLASS, &value);
    g_assert_cmpuint (g_value_get_uint (&value), ==, sub_classes[i]);
    g_value_unset (&value);
  }
}

static void
test_properties_protocol_1 (void)
{
  const guint protocols[] = {
    USBEMU_PROTOCOL_USE_INTERFACE_DESCRIPTOR, 0x01, 0x02,
    USBEMU_PROTOCOL_VENDOR_SPECIFIC,
  };
  UsbemuInterface *interface;
  gsize i;
  GValue value = G_VALUE_INIT;

  interface = usbemu_interface_new ();
  g_test_queue_unref (interface);

  /* default class is USBEMU_PROTOCOL_VENDOR_SPECIFIC. */
  g_assert_cmpuint (usbemu_interface_get_protocol (interface), ==,
                    USBEMU_PROTOCOL_VENDOR_SPECIFIC);

  for (i = 0; i < G_N_ELEMENTS (protocols); i++) {
    usbemu_interface_set_protocol (interface, protocols[i]);
    g_assert_cmpuint (usbemu_interface_get_protocol (interface), ==,
                      protocols[i]);

    g_value_init (&value, G_TYPE_UINT);
    g_object_get_property ((GObject*) interface,
                           USBEMU_INTERFACE_PROP_PROTOCOL, &value);
    g_assert_cmpuint (g_value_get_uint (&value), ==, protocols[i]);
    g_value_unset (&value);
  }
}

int
main (int   argc,
      char *argv[])
{
  setlocale (LC_ALL, "");

  g_test_init (&argc, &argv, NULL);
  g_test_bug_base (PACKAGE_BUGREPORT);

  /* instanciation */

  g_test_add_func ("/UsbemuInterface/instanciation/new",
                   test_instanciation_new_1);
  g_test_add_func ("/UsbemuInterface/instanciation/new-full",
                   test_instanciation_new_full_1);

  /* properties */

  /* name */
  g_test_add_func ("/UsbemuInterface/properties/name",
                   test_properties_name_1);
  /* class */
  g_test_add_func ("/UsbemuInterface/properties/class",
                   test_properties_class_1);
  /* sub-class */
  g_test_add_func ("/UsbemuInterface/properties/sub-class",
                   test_properties_sub_class_1);
  /* protocol */
  g_test_add_func ("/UsbemuInterface/properties/protocol",
                   test_properties_protocol_1);

  return g_test_run ();
}
