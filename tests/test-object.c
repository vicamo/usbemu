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

#include <glib-object.h>

#include "usbemu/usbemu.h"
#include "tests/test-object.h"

typedef struct _UsbemuTestObjectPrivate {
  GObject parent_instance;

  UsbemuError a_enum;
  UsbemuConfigurationAttributes a_flags;
  gchar *a_string;
  guint a_uint;
} UsbemuTestObjectPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (UsbemuTestObject, usbemu_test_object,
                            G_TYPE_OBJECT);

#define USBEMU_TEST_OBJECT_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), USBEMU_TYPE_TEST_OBJECT, \
                                UsbemuTestObjectPrivate))

enum
{
  PROP_0,
  PROP_A_ENUM,
  PROP_A_FLAGS,
  PROP_A_STRING,
  PROP_A_UINT,
  N_PROPERTIES
};

static GParamSpec *props[N_PROPERTIES] = { NULL, };

/* virtual methods for GObjectClass */
static void gobject_class_set_property (GObject *object, guint prop_id,
                                        const GValue *value, GParamSpec *pspec);
static void gobject_class_get_property (GObject *object, guint prop_id,
                                        GValue *value, GParamSpec *pspec);
static void gobject_class_finalize (GObject *object);
/* virtual methods for UsbemuTestObjectClass */
static void usbemu_test_object_class_init (UsbemuTestObjectClass *klass);

static void
gobject_class_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  UsbemuTestObject *tobject = USBEMU_TEST_OBJECT (object);
  UsbemuTestObjectPrivate *priv = USBEMU_TEST_OBJECT_GET_PRIVATE (tobject);

  switch (prop_id) {
    case PROP_A_ENUM:
      priv->a_enum = g_value_get_enum (value);
      break;
    case PROP_A_FLAGS:
      priv->a_flags = g_value_get_flags (value);
      break;
    case PROP_A_STRING:
      priv->a_string = g_value_dup_string (value);
      break;
    case PROP_A_UINT:
      priv->a_uint = g_value_get_uint (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gobject_class_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  UsbemuTestObject *tobject = USBEMU_TEST_OBJECT (object);
  UsbemuTestObjectPrivate *priv = USBEMU_TEST_OBJECT_GET_PRIVATE (tobject);

  switch (prop_id) {
    case PROP_A_ENUM:
      g_value_set_enum (value, priv->a_enum);
      break;
    case PROP_A_FLAGS:
      g_value_set_flags (value, priv->a_flags);
      break;
    case PROP_A_STRING:
      g_value_set_string (value, priv->a_string);
      break;
    case PROP_A_UINT:
      g_value_set_uint (value, priv->a_uint);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gobject_class_finalize (GObject *object)
{
  UsbemuTestObject *tobject = USBEMU_TEST_OBJECT (object);
  UsbemuTestObjectPrivate *priv = USBEMU_TEST_OBJECT_GET_PRIVATE (tobject);

  if (priv->a_string != NULL)
    g_free (priv->a_string);
}

static void
usbemu_test_object_class_init (UsbemuTestObjectClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  /* virtual methods */

  object_class->set_property = gobject_class_set_property;
  object_class->get_property = gobject_class_get_property;
  object_class->finalize = gobject_class_finalize;

  /* properties */

  props[PROP_A_ENUM] =
    g_param_spec_enum (USBEMU_TEST_OBJECT_PROP_A_ENUM,
                       "A enum", "A enum-typed property",
                       USBEMU_TYPE_ERROR, USBEMU_ERROR_FAILED,
                       G_PARAM_READWRITE | \
                         G_PARAM_CONSTRUCT);

  props[PROP_A_FLAGS] =
    g_param_spec_flags (USBEMU_TEST_OBJECT_PROP_A_FLAGS,
                        "A flags", "A flags-typed property",
                        USBEMU_TYPE_CONFIGURATION_ATTRIBUTES,
                        USBEMU_CONFIGURATION_ATTR_RESERVED_7,
                        G_PARAM_READWRITE | \
                          G_PARAM_CONSTRUCT);

  props[PROP_A_STRING] =
    g_param_spec_string (USBEMU_TEST_OBJECT_PROP_A_STRING,
                         "A string", "A string-typed property",
                         "default",
                         G_PARAM_READWRITE | \
                           G_PARAM_CONSTRUCT);

  props[PROP_A_UINT] =
    g_param_spec_uint (USBEMU_TEST_OBJECT_PROP_A_UINT,
                       "A uint", "A uint-typed property",
                       0, G_MAXUINT, 0,
                       G_PARAM_READWRITE | \
                         G_PARAM_CONSTRUCT);

  g_object_class_install_properties (object_class, N_PROPERTIES, props);
}

static void
usbemu_test_object_init (UsbemuTestObject *tobject)
{
  UsbemuTestObjectPrivate *priv = USBEMU_TEST_OBJECT_GET_PRIVATE (tobject);

  priv->a_enum = USBEMU_ERROR_FAILED;
  priv->a_flags = USBEMU_CONFIGURATION_ATTR_RESERVED_7;
  priv->a_string = NULL;
  priv->a_uint = 0;
}

gint
usbemu_test_object_get_enum (UsbemuTestObject *tobject)
{
  g_return_val_if_fail (USBEMU_IS_TEST_OBJECT (tobject), 0);

  return USBEMU_TEST_OBJECT_GET_PRIVATE (tobject)->a_enum;
}

guint
usbemu_test_object_get_flags (UsbemuTestObject *tobject)
{
  g_return_val_if_fail (USBEMU_IS_TEST_OBJECT (tobject), 0);

  return USBEMU_TEST_OBJECT_GET_PRIVATE (tobject)->a_flags;
}

const gchar*
usbemu_test_object_get_string (UsbemuTestObject *tobject)
{
  g_return_val_if_fail (USBEMU_IS_TEST_OBJECT (tobject), NULL);

  return USBEMU_TEST_OBJECT_GET_PRIVATE (tobject)->a_string;
}

guint
usbemu_test_object_get_uint (UsbemuTestObject *tobject)
{
  g_return_val_if_fail (USBEMU_IS_TEST_OBJECT (tobject), 0);

  return USBEMU_TEST_OBJECT_GET_PRIVATE (tobject)->a_uint;
}
