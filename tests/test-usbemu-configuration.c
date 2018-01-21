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
  UsbemuConfiguration *configuration;

  configuration = usbemu_configuration_new ();
  g_assert_nonnull (configuration);
  g_assert_true (USBEMU_IS_CONFIGURATION (configuration));

  g_object_unref (configuration);
}

static void
test_instanciation_new_full_1 (void)
{
  const struct {
    const gchar *name;
    guint attributes;
    guint max_power;
  } entries[] = {
    { NULL,     USBEMU_CONFIGURATION_ATTR_RESERVED_7, 0 },
    { "name 1", USBEMU_CONFIGURATION_ATTR_RESERVED_7 | \
                  USBEMU_CONFIGURATION_ATTR_SELF_POWER, 128 },
  };
  UsbemuConfiguration *configuration;
  gsize i;

  for (i = 0; i < G_N_ELEMENTS (entries); i++) {
    configuration =
        usbemu_configuration_new_full (entries[i].name, entries[i].attributes,
                                       entries[i].max_power);
    g_assert_nonnull (configuration);
    g_assert_true (USBEMU_IS_CONFIGURATION (configuration));

    if (entries[i].name == NULL)
      g_assert_null (usbemu_configuration_get_name (configuration));
    else
      g_assert_cmpstr (usbemu_configuration_get_name (configuration), ==,
                       entries[i].name);
    g_assert_cmpuint (usbemu_configuration_get_attributes (configuration), ==,
                      entries[i].attributes);
    g_assert_cmpuint (usbemu_configuration_get_max_power (configuration), ==,
                      entries[i].max_power);

    g_object_unref (configuration);
  }
}

static void
test_instanciation_new_from_string_1 (void)
{
  const gchar* strings[] = {
    NULL,
    "",
    USBEMU_CONFIGURATION_PROP_NAME "=my-name",
    USBEMU_CONFIGURATION_PROP_ATTRIBUTES "=REMOTE_WAKEUP",
    USBEMU_CONFIGURATION_PROP_ATTRIBUTES "=REMOTE_WAKEUP|SELF_POWER",
    USBEMU_CONFIGURATION_PROP_NAME "=my-name " USBEMU_CONFIGURATION_PROP_ATTRIBUTES "=REMOTE_WAKEUP",
    "--interface",
    USBEMU_CONFIGURATION_PROP_NAME "=my-name --interface",
    "--interface --interface",
    "--interface --alternate-setting",
    "--interface --alternate-setting --alternate-setting",
    "--interface --alternate-setting --interface",
    "--interface --alternate-setting --interface --alternate-setting",
  };
  UsbemuConfiguration *configuration;
  GError *error = NULL;
  gsize i;

  for (i = 0; i < G_N_ELEMENTS (strings); i++) {
    g_test_message ("testing: %s", strings[i]);
    configuration = usbemu_configuration_new_from_string (strings[i], &error);
    g_assert_nonnull (configuration);
    g_assert_true (USBEMU_IS_CONFIGURATION (configuration));
    g_assert_null (error);
    g_object_unref (configuration);
  }
}

static void
test_instanciation_new_from_string_2 (void)
{
  const struct {
    gchar *description;
    GQuark error_domain;
    gint error_code;
  } cases[] = {
    { "object-type=NoSuchType",
      USBEMU_ERROR, USBEMU_ERROR_INVALID_TYPE },
    { "object-type=UsbemuConfiguration",
      USBEMU_ERROR, USBEMU_ERROR_INVALID_TYPE },
  };
  UsbemuConfiguration *configuration;
  GError *error = NULL;
  gsize i;

  for (i = 0; i < G_N_ELEMENTS (cases); i++) {
    g_test_message ("testing: %s", cases[i].description);
    configuration = usbemu_configuration_new_from_string (cases[i].description, &error);
    g_assert_null (configuration);
    if (cases[i].error_domain == 0)
      g_assert_null (error);
    else {
      g_assert_nonnull (error);
      g_test_message ("got error %s:%d", g_quark_to_string (error->domain), error->code);
      g_assert_cmpint (error->domain, ==, cases[i].error_domain);
      g_assert_cmpint (error->code, ==, cases[i].error_code);
      g_error_free (error);
      error = NULL;
    }
  }
}

static void
test_properties_name_1 (void)
{
  const gchar *names[] = {
    "1", NULL,
  };
  UsbemuConfiguration *configuration;
  gsize i;
  GValue value = G_VALUE_INIT;

  configuration = usbemu_configuration_new ();
  g_test_queue_unref (configuration);

  /* default name is NULL. */
  g_assert_null (usbemu_configuration_get_name (configuration));

  for (i = 0; i < G_N_ELEMENTS (names); i++) {
    g_value_init (&value, G_TYPE_STRING);

    usbemu_configuration_set_name (configuration, names[i]);
    g_object_get_property ((GObject*) configuration,
                           USBEMU_CONFIGURATION_PROP_NAME, &value);
    if (names[i] == NULL) {
      g_assert_null (usbemu_configuration_get_name (configuration));
      g_assert_null (g_value_get_string (&value));
    } else {
      g_assert_cmpstr (usbemu_configuration_get_name (configuration), ==,
                       names[i]);
      g_assert_cmpstr (g_value_get_string (&value), ==, names[i]);
    }

    g_value_unset (&value);
  }
}

static void
test_properties_attributes_1 (void)
{
  const guint8 attributes[] = {
    USBEMU_CONFIGURATION_ATTR_RESERVED_7 | \
      USBEMU_CONFIGURATION_ATTR_REMOTE_WAKEUP,
    USBEMU_CONFIGURATION_ATTR_RESERVED_7 | \
      USBEMU_CONFIGURATION_ATTR_SELF_POWER,
    USBEMU_CONFIGURATION_ATTR_RESERVED_7,
  };
  UsbemuConfiguration *configuration;
  gsize i;
  GValue value = G_VALUE_INIT;

  configuration = usbemu_configuration_new ();
  g_test_queue_unref (configuration);

  /* default attributes is USBEMU_CONFIGURATION_ATTR_RESERVED_7. */
  g_assert_cmpuint (usbemu_configuration_get_attributes (configuration), ==,
                    USBEMU_CONFIGURATION_ATTR_RESERVED_7);

  for (i = 0; i < G_N_ELEMENTS (attributes); i++) {

    usbemu_configuration_set_attributes (configuration, attributes[i]);
    g_assert_cmpuint (usbemu_configuration_get_attributes (configuration), ==,
                      attributes[i]);

    g_value_init (&value, G_TYPE_UINT);
    g_object_get_property ((GObject*) configuration,
                           USBEMU_CONFIGURATION_PROP_ATTRIBUTES, &value);
    g_assert_cmpuint (g_value_get_uint (&value), ==, attributes[i]);
    g_value_unset (&value);
  }
}

static void
test_properties_max_power_1 (void)
{
  const guint8 max_powers[] = {
    USBEMU_CONFIGURATION_ATTR_RESERVED_7 | \
      USBEMU_CONFIGURATION_ATTR_REMOTE_WAKEUP,
    USBEMU_CONFIGURATION_ATTR_RESERVED_7 | \
      USBEMU_CONFIGURATION_ATTR_SELF_POWER,
    USBEMU_CONFIGURATION_ATTR_RESERVED_7,
  };
  UsbemuConfiguration *configuration;
  gsize i;
  GValue value = G_VALUE_INIT;

  configuration = usbemu_configuration_new ();
  g_test_queue_unref (configuration);

  /* default max-power is 2. */
  g_assert_cmpuint (usbemu_configuration_get_max_power (configuration), ==, 2);

  for (i = 0; i < G_N_ELEMENTS (max_powers); i++) {
    usbemu_configuration_set_max_power (configuration, max_powers[i]);
    g_assert_cmpuint (usbemu_configuration_get_max_power (configuration), ==,
                      max_powers[i]);

    g_value_init (&value, G_TYPE_UINT);
    g_object_get_property ((GObject*) configuration,
                           USBEMU_CONFIGURATION_PROP_MAX_POWER, &value);
    g_assert_cmpuint (g_value_get_uint (&value), ==, max_powers[i]);
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

  g_test_add_func ("/UsbemuConfiguration/instanciation/new",
                   test_instanciation_new_1);
  g_test_add_func ("/UsbemuConfiguration/instanciation/new-full",
                   test_instanciation_new_full_1);
  g_test_add_func ("/UsbemuConfiguration/instanciation/new-from-string/1",
                   test_instanciation_new_from_string_1);
  g_test_add_func ("/UsbemuConfiguration/instanciation/new-from-string/2",
                   test_instanciation_new_from_string_2);

  /* properties */

  /* name */
  g_test_add_func ("/UsbemuConfiguration/properties/name",
                   test_properties_name_1);
  /* attributes */
  g_test_add_func ("/UsbemuConfiguration/properties/attributes",
                   test_properties_attributes_1);
  /* max-power */
  g_test_add_func ("/UsbemuConfiguration/properties/max-power",
                   test_properties_max_power_1);

  return g_test_run ();
}
