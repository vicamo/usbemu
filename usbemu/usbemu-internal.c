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

#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "usbemu/usbemu.h"
#include "usbemu/usbemu-internal.h"

static gboolean
_transform_string_to_type (gchar  *str,
                           GType   type,
                           GValue *value)
{
  if (type == G_TYPE_STRING) {
    g_value_set_string (value, str);
    return TRUE;
  }

  if (type == G_TYPE_UINT) {
    g_value_set_uint (value, strtoul (str, NULL, 0));
    return TRUE;
  }

  if (G_TYPE_IS_ENUM (type)) {
    GEnumValue *enum_value;

    enum_value = usbemu_enum_get_value_by_name_or_nick (type, str);
    if (enum_value == NULL)
      return FALSE;

    g_value_set_enum (value, enum_value->value);
    return TRUE;
  }

  if (G_TYPE_IS_FLAGS (type)) {
    guint flags = 0;

    if (!usbemu_flags_from_string (type, str, &flags))
      return FALSE;

    g_value_set_flags (value, flags);
    return TRUE;
  }

  return FALSE;
}

static gpointer
_ensure_types_inner (gpointer data G_GNUC_UNUSED)
{
  return NULL;
}

static void
_ensure_types ()
{
  static GOnce once = G_ONCE_INIT;

  g_once (&once, _ensure_types_inner, NULL);
}

/* _extract_properties_from_argv:
 * @argv: (inout) (array zero-terminated=1):
 * @base_type: (inout):
 * @type_key: (in):
 * @keys: (out caller-allocates) (type GArray(gchar)):
 * @values: (out caller-allocates) (type GArray(GValue)):
 * @error: (out callee-allocates) (optional):
 */
static gboolean
_extract_properties_from_argv (gchar       ***argv,
                               GType         *base_type,
                               const gchar   *type_key,
                               GArray        *keys,
                               GArray        *values,
                               GError       **error)
{
  GType type;
  gpointer klass;
  gchar **strv, *k, *v;
  gboolean ret = TRUE;
  GParamSpec *pspec;
  GValue *pvalue;

  type = *base_type;
  klass = g_type_class_ref (type);
  g_assert_nonnull (klass);

  strv = *argv;
  while (*strv != NULL) {
    k = *strv;
    if ((*k == '-') && (*(k + 1) == '-'))
      break;

    ++strv;

    v = strchr (k, '=');
    if ((v == NULL) || (*(v + 1) == '\0') || (v == k)) {
      g_set_error (error, USBEMU_ERROR, USBEMU_ERROR_SYNTAX_ERROR,
                   "syntax error at '%s'", k);
      ret = FALSE;
      break;
    }

    *v = '\0'; ++v;
    if ((type_key != NULL) && (g_ascii_strcasecmp (k, type_key) == 0)) {
      /* ensure all instanciable types have been registered. */
      _ensure_types ();

      type = g_type_from_name (v);
      if ((type == 0) || !g_type_is_a (type, *base_type)) {
        g_set_error (error, USBEMU_ERROR, USBEMU_ERROR_INSTANCIATION_FAILED,
                     "invalid type name '%s' for base type %s",
                     v, g_type_name (*base_type));
        ret = FALSE;
        break;
      }

      g_type_class_unref (klass);
      klass = g_type_class_ref (type);
      g_assert_nonnull (klass);
      *base_type = type;
      continue;
    }

    pspec = g_object_class_find_property (klass, k);
    if (pspec == NULL) {
      g_set_error (error, USBEMU_ERROR, USBEMU_ERROR_INSTANCIATION_FAILED,
                   "no such property '%s' for type %s", k, g_type_name (type));
      ret = FALSE;
      break;
    }

    /* g_array_append_val() takes only variables. */
    k = g_strdup (k);
    g_array_append_val (keys, k);

    g_array_set_size (values, values->len + 1);
    pvalue = &g_array_index (values, GValue, values->len - 1);
    g_value_init (pvalue, pspec->value_type);

    /* TODO: use g_value_transform? */
    if (!_transform_string_to_type (v, pspec->value_type, pvalue)) {
      g_set_error (error, USBEMU_ERROR, USBEMU_ERROR_SYNTAX_ERROR,
                   "invalid value '%s' for type %s",
                   v, g_type_name (pspec->value_type));
      ret = FALSE;
      break;
    }
  }

  g_type_class_unref (klass);
  *argv = strv;

  return ret;
}

void
_usbemu_free_dereferenced (gchar **data)
{
  g_free (*data);
}

void
_usbemu_object_unref_dereferenced (GObject **object)
{
  g_object_unref (*object);
}

GObject*
_usbemu_object_new_from_argv (gchar       ***argv,
                              GType         *base_type,
                              const gchar   *type_key,
                              GError       **error)
{
  GArray *keys;
  GArray *values;
  GObject *object = NULL;

  keys = g_array_new (FALSE, TRUE, sizeof (gchar*));
  values = g_array_new (FALSE, TRUE, sizeof (GValue));
  g_array_set_clear_func (keys, (GDestroyNotify) _usbemu_free_dereferenced);
  g_array_set_clear_func (values, (GDestroyNotify) g_value_unset);

  if (_extract_properties_from_argv (argv, base_type, type_key,
                                     keys, values, error)) {
    object = g_object_new_with_properties (*base_type, keys->len,
                                           (const gchar**) keys->data,
                                           (GValue*) values->data);
    if (object == NULL) {
      g_set_error (error, USBEMU_ERROR, USBEMU_ERROR_INSTANCIATION_FAILED,
                   "failed to create device instance of type %s",
                   g_type_name (*base_type));
    }
  }

  g_array_free (keys, TRUE);
  g_array_free (values, TRUE);

  return object;
}
