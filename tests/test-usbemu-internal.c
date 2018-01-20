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
#include "usbemu/usbemu-internal.h"
#include "tests/test-object.h"

static void
check_valid_object (GObject *object,
                    GType    type,
                    GError  *error,
                    GType    expected_type)
{
  g_assert_nonnull (object);
  g_assert_cmpint (G_OBJECT_TYPE (object), ==, expected_type);
  g_assert_cmpint (type, ==, expected_type);
  g_assert_null (error);
}

static void
check_invalid_object (GObject *object,
                      GType    type,
                      GError  *error,
                      GType    expected_type,
                      GQuark   error_domain,
                      gint     error_code)
{
  g_assert_null (object);
  g_assert_cmpint (type, ==, expected_type);
  g_assert_nonnull (error);
  g_assert_cmpint (error->domain, ==, error_domain);
  g_assert_cmpint (error->code, ==, error_code);
}

static void
check_object_and_cleanup (GObject  *object,
                          GType     type,
                          GError  **error,
                          GType     expected_type,
                          GQuark    error_domain,
                          gint      error_code)
{
  if (error_domain == 0) {
    check_valid_object (object, type, *error, expected_type);
    g_object_unref (object);
  } else {
    check_invalid_object (object, type, *error, expected_type,
                          error_domain, error_code);
    g_error_free (*error);
    *error = NULL;
  }
}

static void
test_object_new_from_argv__null (void)
{
  gchar *null = NULL;
  gchar** cases[] = {
    NULL,
    &null,
  };
  GObject *object;
  GType base_type = USBEMU_TYPE_TEST_OBJECT;
  GError *error = NULL;
  gsize i;

  for (i = 0; i < G_N_ELEMENTS (cases); i++) {
    g_test_message ("testing cases[%zu]", i);

    object = _usbemu_object_new_from_argv (&cases[i], &base_type, NULL, &error);
    check_object_and_cleanup (object, base_type, &error,
                              USBEMU_TYPE_TEST_OBJECT, 0, 0);
  }
}

static void
test_object_new_from_argv__empty_string (void)
{
  gchar* empty_and_null[] = {
    "",
    NULL,
  };
  struct {
    gchar **strv;
    GQuark error_domain;
    gint error_code;
  } cases[] = {
    { empty_and_null, USBEMU_ERROR, USBEMU_ERROR_SYNTAX_ERROR, },
  };
  GObject *object;
  GType base_type = USBEMU_TYPE_TEST_OBJECT;
  GError *error = NULL;
  gsize i;

  for (i = 0; i < G_N_ELEMENTS (cases); i++) {
    g_test_message ("testing cases[%zu]", i);

    object = _usbemu_object_new_from_argv (&cases[i].strv, &base_type,
                                           NULL, &error);
    check_object_and_cleanup (object, base_type, &error,
                              USBEMU_TYPE_TEST_OBJECT,
                              cases[i].error_domain, cases[i].error_code);
  }
}

static void
test_object_new_from_argv__object_type (void)
{
  struct {
    GType result_type;
    GType base_type;
    const gchar *string;
  } cases[] = {
    /* splitted as { NULL }, so it should be valid. */
    { G_TYPE_OBJECT, G_TYPE_OBJECT, "", },
    /* specify object-type */
    { G_TYPE_OBJECT, G_TYPE_OBJECT, "object-type=GObject", },
    /* specify a derived object-type */
    { USBEMU_TYPE_TEST_OBJECT, G_TYPE_OBJECT, "object-type=UsbemuTestObject", },
    /* splitted as { NULL }, so it should be valid. */
    { USBEMU_TYPE_TEST_OBJECT, USBEMU_TYPE_TEST_OBJECT, "", },
    /* specify object-type */
    { USBEMU_TYPE_TEST_OBJECT, USBEMU_TYPE_TEST_OBJECT,
      "object-type=UsbemuTestObject", },
    /* specify an invalid object-type */
    { 0, G_TYPE_OBJECT, "object-type=NoSuchType", },
    /* specify a base object-type */
    { 0, USBEMU_TYPE_TEST_OBJECT, "object-type=GObject", },
  };
  GObject *object;
  GType base_type;
  GError *error = NULL;
  gchar **strv, **strv_keep;
  gsize i;

  for (i = 0; i < G_N_ELEMENTS (cases); i++) {
    g_test_message ("testing cases[%zu]", i);

    base_type = cases[i].base_type;
    strv_keep = strv = g_strsplit (cases[i].string, " ", 0);
    object = _usbemu_object_new_from_argv (&strv, &base_type,
                                           "object-type", &error);
    if (cases[i].result_type != 0) {
      check_object_and_cleanup (object, base_type, &error,
                                cases[i].result_type, 0, 0);
    } else {
      check_object_and_cleanup (object, base_type, &error,
                                cases[i].base_type, USBEMU_ERROR,
                                USBEMU_ERROR_INSTANCIATION_FAILED);
    }

    g_strfreev (strv_keep);
  }
}

static void
test_object_new_from_argv__property_syntax (void)
{
  const gchar* cases[] = {
    USBEMU_TEST_OBJECT_PROP_A_UINT,
    USBEMU_TEST_OBJECT_PROP_A_UINT "=",
    "=",
    "=3",
  };
  GObject *object;
  GType base_type = USBEMU_TYPE_TEST_OBJECT;
  GError *error = NULL;
  gchar **strv, **strv_keep;
  gsize i;

  for (i = 0; i < G_N_ELEMENTS (cases); i++) {
    g_test_message ("testing cases[%zu]", i);

    strv_keep = strv = g_strsplit (cases[i], " ", 0);
    object = _usbemu_object_new_from_argv (&strv, &base_type, NULL, &error);
    check_object_and_cleanup (object, base_type, &error,
                              USBEMU_TYPE_TEST_OBJECT,
                              USBEMU_ERROR, USBEMU_ERROR_SYNTAX_ERROR);

    g_strfreev (strv_keep);
  }
}

static void
test_object_new_from_argv__property_type_enum (void)
{
  struct {
    const gchar *str;
    gint value;
  } cases[] = {
    { USBEMU_TEST_OBJECT_PROP_A_ENUM "=USBEMU_ERROR_SYNTAX_ERROR",
      USBEMU_ERROR_SYNTAX_ERROR },
    { USBEMU_TEST_OBJECT_PROP_A_ENUM "=SyntaxError",
      USBEMU_ERROR_SYNTAX_ERROR },
  };
  GObject *object;
  GType base_type = USBEMU_TYPE_TEST_OBJECT;
  GError *error = NULL;
  gchar **strv, **strv_keep;
  gsize i;

  for (i = 0; i < G_N_ELEMENTS (cases); i++) {
    g_test_message ("testing cases[%zu]", i);

    strv_keep = strv = g_strsplit (cases[i].str, " ", 0);
    object = _usbemu_object_new_from_argv (&strv, &base_type, NULL, &error);
    check_valid_object (object, base_type, error, USBEMU_TYPE_TEST_OBJECT);
    g_assert_cmpint (usbemu_test_object_get_enum ((UsbemuTestObject*) object),
                     ==, cases[i].value);

    g_object_unref (object);
    g_strfreev (strv_keep);
  }
}

static void
test_object_new_from_argv__property_type_flags (void)
{
  struct {
    const gchar *str;
    guint value;
  } cases[] = {
    { USBEMU_TEST_OBJECT_PROP_A_FLAGS "=USBEMU_CONFIGURATION_ATTR_REMOTE_WAKEUP",
      USBEMU_CONFIGURATION_ATTR_REMOTE_WAKEUP },
    { USBEMU_TEST_OBJECT_PROP_A_FLAGS "=REMOTE_WAKEUP",
      USBEMU_CONFIGURATION_ATTR_REMOTE_WAKEUP },
    { USBEMU_TEST_OBJECT_PROP_A_FLAGS "=" \
        "USBEMU_CONFIGURATION_ATTR_REMOTE_WAKEUP|USBEMU_CONFIGURATION_ATTR_SELF_POWER",
      USBEMU_CONFIGURATION_ATTR_REMOTE_WAKEUP | USBEMU_CONFIGURATION_ATTR_SELF_POWER },
    { USBEMU_TEST_OBJECT_PROP_A_FLAGS "=" \
        "USBEMU_CONFIGURATION_ATTR_REMOTE_WAKEUP|SELF_POWER",
      USBEMU_CONFIGURATION_ATTR_REMOTE_WAKEUP | USBEMU_CONFIGURATION_ATTR_SELF_POWER },
    { USBEMU_TEST_OBJECT_PROP_A_FLAGS "=" \
        "REMOTE_WAKEUP|USBEMU_CONFIGURATION_ATTR_SELF_POWER",
      USBEMU_CONFIGURATION_ATTR_REMOTE_WAKEUP | USBEMU_CONFIGURATION_ATTR_SELF_POWER },
    { USBEMU_TEST_OBJECT_PROP_A_FLAGS "=" \
        "REMOTE_WAKEUP|SELF_POWER",
      USBEMU_CONFIGURATION_ATTR_REMOTE_WAKEUP | USBEMU_CONFIGURATION_ATTR_SELF_POWER },
  };
  GObject *object;
  GType base_type = USBEMU_TYPE_TEST_OBJECT;
  GError *error = NULL;
  gchar **strv, **strv_keep;
  gsize i;

  for (i = 0; i < G_N_ELEMENTS (cases); i++) {
    g_test_message ("testing cases[%zu]", i);

    strv_keep = strv = g_strsplit (cases[i].str, " ", 0);
    object = _usbemu_object_new_from_argv (&strv, &base_type, NULL, &error);
    check_valid_object (object, base_type, error, USBEMU_TYPE_TEST_OBJECT);
    g_assert_cmpuint (usbemu_test_object_get_flags ((UsbemuTestObject*) object),
                      ==, cases[i].value);

    g_object_unref (object);
    g_strfreev (strv_keep);
  }
}

static void
test_object_new_from_argv__property_type_string (void)
{
  struct {
    const gchar *str;
    const gchar *value;
  } cases[] = {
    { USBEMU_TEST_OBJECT_PROP_A_STRING "=hi", "hi", },
  };
  GObject *object;
  GType base_type = USBEMU_TYPE_TEST_OBJECT;
  GError *error = NULL;
  gchar **strv, **strv_keep;
  gsize i;

  for (i = 0; i < G_N_ELEMENTS (cases); i++) {
    g_test_message ("testing cases[%zu]", i);

    strv_keep = strv = g_strsplit (cases[i].str, " ", 0);
    object = _usbemu_object_new_from_argv (&strv, &base_type, NULL, &error);
    check_valid_object (object, base_type, error, USBEMU_TYPE_TEST_OBJECT);
    g_assert_cmpstr (usbemu_test_object_get_string ((UsbemuTestObject*) object),
                     ==, cases[i].value);

    g_object_unref (object);
    g_strfreev (strv_keep);
  }
}

static void
test_object_new_from_argv__property_type_uint (void)
{
  struct {
    const gchar *str;
    guint value;
  } cases[] = {
    { USBEMU_TEST_OBJECT_PROP_A_UINT "=0", 0, },
    { USBEMU_TEST_OBJECT_PROP_A_UINT "=1234", 1234, },
    { NULL }, /* place holder for G_MAXUINT case. */
  };
  gchar buf[32];
  GObject *object;
  GType base_type = USBEMU_TYPE_TEST_OBJECT;
  GError *error = NULL;
  gchar **strv, **strv_keep;
  gsize i;

  g_snprintf (buf, sizeof (buf),
              USBEMU_TEST_OBJECT_PROP_A_UINT "=%u", G_MAXUINT);
  cases[2].str = buf;
  cases[2].value = G_MAXUINT;

  for (i = 0; i < G_N_ELEMENTS (cases); i++) {
    g_test_message ("testing cases[%zu]", i);

    strv_keep = strv = g_strsplit (cases[i].str, " ", 0);
    object = _usbemu_object_new_from_argv (&strv, &base_type, NULL, &error);
    check_valid_object (object, base_type, error, USBEMU_TYPE_TEST_OBJECT);
    g_assert_cmpuint (usbemu_test_object_get_uint ((UsbemuTestObject*) object),
                      ==, cases[i].value);

    g_object_unref (object);
    g_strfreev (strv_keep);
  }
}

static void
test_object_new_from_argv__multiple_properties (void)
{
  const gchar *str =
        USBEMU_TEST_OBJECT_PROP_A_ENUM "=USBEMU_ERROR_SYNTAX_ERROR "
        USBEMU_TEST_OBJECT_PROP_A_FLAGS "=REMOTE_WAKEUP|SELF_POWER "
        USBEMU_TEST_OBJECT_PROP_A_STRING "=hello "
        USBEMU_TEST_OBJECT_PROP_A_UINT "=3";
  GObject *object;
  GType base_type = USBEMU_TYPE_TEST_OBJECT;
  GError *error = NULL;
  gchar **strv, **strv_keep;

  strv_keep = strv = g_strsplit (str, " ", 0);
  object = _usbemu_object_new_from_argv (&strv, &base_type, NULL, &error);
  check_valid_object (object, base_type, error, USBEMU_TYPE_TEST_OBJECT);
  g_assert_cmpint (usbemu_test_object_get_enum ((UsbemuTestObject*) object),
                   ==, USBEMU_ERROR_SYNTAX_ERROR);
  g_assert_cmpuint (usbemu_test_object_get_flags ((UsbemuTestObject*) object),
                    ==,
                    USBEMU_CONFIGURATION_ATTR_REMOTE_WAKEUP | \
                      USBEMU_CONFIGURATION_ATTR_SELF_POWER);
  g_assert_cmpstr (usbemu_test_object_get_string ((UsbemuTestObject*) object),
                   ==, "hello");
  g_assert_cmpuint (usbemu_test_object_get_uint ((UsbemuTestObject*) object),
                    ==, 3);

  g_object_unref (object);
  g_strfreev (strv_keep);
}

int
main (int   argc,
      char *argv[])
{
  setlocale (LC_ALL, "");

  g_test_init (&argc, &argv, NULL);
  g_test_bug_base (PACKAGE_BUGREPORT);

  /* register our test object type before test cases. */
  g_type_ensure (USBEMU_TYPE_TEST_OBJECT);

  g_test_add_func ("/internal/object_new_from_argv/null",
                   test_object_new_from_argv__null);
  g_test_add_func ("/internal/object_new_from_argv/empty-string",
                   test_object_new_from_argv__empty_string);
  g_test_add_func ("/internal/object_new_from_argv/object-type",
                   test_object_new_from_argv__object_type);
  g_test_add_func ("/internal/object_new_from_argv/property-syntax",
                   test_object_new_from_argv__property_syntax);
  g_test_add_func ("/internal/object_new_from_argv/property-type-enum",
                   test_object_new_from_argv__property_type_enum);
  g_test_add_func ("/internal/object_new_from_argv/property-type-flags",
                   test_object_new_from_argv__property_type_flags);
  g_test_add_func ("/internal/object_new_from_argv/property-type-string",
                   test_object_new_from_argv__property_type_string);
  g_test_add_func ("/internal/object_new_from_argv/property-type-uint",
                   test_object_new_from_argv__property_type_uint);
  g_test_add_func ("/internal/object_new_from_argv/multiple-properties",
                   test_object_new_from_argv__multiple_properties);

  return g_test_run ();
}
