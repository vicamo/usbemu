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

#include <glib.h>

#include "usbemu/usbemu-configuration.h"
#include "usbemu/usbemu-enums.h"
#include "usbemu/usbemu-interface.h"

#include "usbemu/internal/utils.h"

/**
 * SECTION:usbemu-configuration
 * @title: UsbemuConfiguration
 * @short_description: GObject representing USB device configuration descriptor.
 * @include: usbemu/usbemu.h
 *
 * TODO.
 */

/**
 * UsbemuConfiguration:
 *
 * GObject representing USB device configuration descriptor.
 */

/**
 * UsbemuConfigurationClass:
 * @parent_class: The parent class.
 *
 * Class structure for UsbemuConfiguration.
 */

struct _UsbemuConfiguration {
  GObject parent_instance;

  guint bConfigurationValue;
  gchar *name;
  guint bmAttributes;
  guint bMaxPower;

  UsbemuDevice *device;
  GPtrArray *interface_groups;
};

G_DEFINE_TYPE (UsbemuConfiguration, usbemu_configuration, G_TYPE_OBJECT)

enum
{
  PROP_0,
  PROP_NAME,
  PROP_ATTRIBUTES,
  PROP_MAX_POWER,
  N_PROPERTIES
};

static GParamSpec *props[N_PROPERTIES] = { NULL, };

#define USBEMU_CONFIGURATION_PROP_NAME__DEFAULT NULL
/* D7 is reserved and must be set to one for historical reasons. */
#define USBEMU_CONFIGURATION_PROP_ATTRIBUTES__DEFAULT \
    USBEMU_CONFIGURATION_ATTR_RESERVED_7
#define USBEMU_CONFIGURATION_PROP_MAX_POWER__DEFAULT 2

/* virtual methods for GObjectClass */
static void gobject_class_set_property (GObject *object, guint prop_id,
                                        const GValue *value, GParamSpec *pspec);
static void gobject_class_get_property (GObject *object, guint prop_id,
                                        GValue *value, GParamSpec *pspec);
static void gobject_class_dispose (GObject *object);
static void gobject_class_finalize (GObject *object);
/* virtual methods for UsbemuConfigurationClass */
static void usbemu_configuration_class_init (UsbemuConfigurationClass *configuration_class);

static void
gobject_class_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  UsbemuConfiguration *configuration = USBEMU_CONFIGURATION (object);

  switch (prop_id) {
    case PROP_NAME:
      if (configuration->name)
        g_free (configuration->name);
      configuration->name = g_value_dup_string (value);
      break;
    case PROP_ATTRIBUTES:
      configuration->bmAttributes = g_value_get_flags (value);
      break;
    case PROP_MAX_POWER:
      configuration->bMaxPower = g_value_get_uint (value);
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
  UsbemuConfiguration *configuration = USBEMU_CONFIGURATION (object);

  switch (prop_id) {
    case PROP_NAME:
      g_value_set_string (value, configuration->name);
      break;
    case PROP_ATTRIBUTES:
      g_value_set_flags (value, configuration->bmAttributes);
      break;
    case PROP_MAX_POWER:
      g_value_set_uint (value, configuration->bMaxPower);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gobject_class_dispose (GObject *object)
{
  UsbemuConfiguration *configuration = USBEMU_CONFIGURATION (object);

  if (configuration->interface_groups != NULL) {
    g_ptr_array_unref (configuration->interface_groups);
    configuration->interface_groups = NULL;
  }

  g_clear_object (&configuration->device);
}

static void
gobject_class_finalize (GObject *object)
{
  UsbemuConfiguration *configuration = USBEMU_CONFIGURATION (object);

  if (configuration->name)
    g_free (configuration->name);
}

static void
usbemu_configuration_class_init (UsbemuConfigurationClass *configuration_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (configuration_class);

  /* virtual methods */

  object_class->set_property = gobject_class_set_property;
  object_class->get_property = gobject_class_get_property;
  object_class->dispose = gobject_class_dispose;
  object_class->finalize = gobject_class_finalize;

  /* properties */

  /**
   * UsbemuConfiguration:name:
   */
  props[PROP_NAME] =
        g_param_spec_string (USBEMU_CONFIGURATION_PROP_NAME,
                             "Name", "Name",
                             USBEMU_CONFIGURATION_PROP_NAME__DEFAULT,
                             G_PARAM_READWRITE | \
                               G_PARAM_CONSTRUCT);

  /**
   * UsbemuConfiguration:attributes:
   */
  props[PROP_ATTRIBUTES] =
        g_param_spec_flags (USBEMU_CONFIGURATION_PROP_ATTRIBUTES,
                            "Attributes", "Attributes",
                            USBEMU_TYPE_CONFIGURATION_ATTRIBUTES,
                            USBEMU_CONFIGURATION_PROP_ATTRIBUTES__DEFAULT,
                            G_PARAM_READWRITE | \
                              G_PARAM_CONSTRUCT);

  /**
   * UsbemuConfiguration:max-power:
   */
  props[PROP_MAX_POWER] =
        g_param_spec_uint (USBEMU_CONFIGURATION_PROP_MAX_POWER,
                           "Max Power", "Max Power",
                           0, G_MAXUINT,
                           USBEMU_CONFIGURATION_PROP_MAX_POWER__DEFAULT,
                           G_PARAM_READWRITE | \
                             G_PARAM_CONSTRUCT);

  g_object_class_install_properties (object_class, N_PROPERTIES, props);
}

static void
usbemu_configuration_init (UsbemuConfiguration *configuration)
{
  configuration->bConfigurationValue = 0;
  configuration->name = USBEMU_CONFIGURATION_PROP_NAME__DEFAULT;
  configuration->bmAttributes = USBEMU_CONFIGURATION_PROP_ATTRIBUTES__DEFAULT;
  configuration->bMaxPower = USBEMU_CONFIGURATION_PROP_MAX_POWER__DEFAULT;
  configuration->device = NULL;

  configuration->interface_groups =
        g_ptr_array_new_with_free_func ((GDestroyNotify) g_ptr_array_unref);
}

/**
 * usbemu_configuration_new:
 *
 * Create a new #UsbemuConfiguration instance.
 *
 * Returns: (transfer full) (type UsbemuConfiguration): The constructed device
 *          configuration object or %NULL.
 */
UsbemuConfiguration*
usbemu_configuration_new ()
{
  return g_object_new (USBEMU_TYPE_CONFIGURATION, NULL);
}

/**
 * usbemu_configuration_new_full:
 * @name: (in): the new name or %NULL to remove it..
 * @attributes: flags of #UsbemuConfigurationAttributes.
 * @max_power: maximum power consumption under this configuration in mA.
 *
 * Create a new #UsbemuConfiguration instance with details specified.
 *
 * Returns: (transfer full) (type UsbemuConfiguration): The constructed device
 *          configuration object or %NULL.
 */
UsbemuConfiguration*
usbemu_configuration_new_full (const gchar *name,
                               guint        attributes,
                               guint        max_power)
{
  return g_object_new (USBEMU_TYPE_CONFIGURATION,
                       USBEMU_CONFIGURATION_PROP_NAME, name,
                       USBEMU_CONFIGURATION_PROP_ATTRIBUTES, attributes,
                       USBEMU_CONFIGURATION_PROP_MAX_POWER, max_power,
                       NULL);
}

UsbemuConfiguration*
_usbemu_configuration_new_from_argv_inner (gchar    ***argv,
                                           GError    **error,
                                           gboolean    allow_remaining)
{
  GType configuration_type;
  UsbemuConfiguration *configuration;
  gchar **strv;
  UsbemuInterface *interface;
  guint interface_number;

  configuration_type = USBEMU_TYPE_CONFIGURATION;
  configuration = (UsbemuConfiguration*)
        _usbemu_object_new_from_argv (argv, &configuration_type, NULL, error);
  if (configuration == NULL)
    return NULL;

  if (*argv == NULL)
    return configuration;

  interface_number = 0;

  strv = *argv;
  while (*strv != NULL) {
    if (g_ascii_strcasecmp (*strv, "--interface") == 0) {
      ++strv;
      interface_number = configuration->interface_groups->len;
      continue;
    } else if (g_ascii_strcasecmp (*strv, "--alternate-setting") == 0) {
      ++strv;
      interface = _usbemu_interface_new_from_argv_inner (&strv, error, TRUE);
      if (interface != NULL) {
        usbemu_configuration_add_alternate_interface (configuration,
                                                      interface_number,
                                                      interface);
        continue;
      }
    } else if (allow_remaining && (**strv == '-')) {
      break;
    } else {
      g_set_error (error, USBEMU_ERROR, USBEMU_ERROR_SYNTAX_ERROR,
                   "unknown argument '%s'", *strv);
    }

    g_object_unref (configuration);
    configuration = NULL;
    break;
  }
  *argv = strv;

  return configuration;
}

/**
 * usbemu_configuration_new_from_argv:
 * @argv: (in) (optional) (array zero-terminated=1): configuration description.
 * @error: (out) (optional): return location for error.
 *
 * Create #UsbemuConfiguration from tokenized command line string. See
 * usbemu_device_new_from_string() for valid syntax.
 *
 * A generic interface object is created when %NULL or an empty strv is
 * passed.
 *
 * Returns: (transfer full): a newly created #UsbemuConfiguration object or
 *          %NULL if failed.
 */
UsbemuConfiguration*
usbemu_configuration_new_from_argv (gchar  **argv,
                                    GError **error)
{
  return _usbemu_configuration_new_from_argv_inner (&argv, error, FALSE);
}

/**
 * usbemu_configuration_new_from_string:
 * @str: (in) (optional): command line like configuration description.
 * @error: (out) (optional): return location for error.
 *
 * Create #UsbemuConfiguration from a command line like formated string. See
 * usbemu_device_new_from_string() for valid syntax.
 *
 * A generic configuration object is created when %NULL or an empty string is
 * passed.
 *
 * Returns: (transfer full): a newly created #UsbemuConfiguration object or
 *          %NULL if failed.
 */
UsbemuConfiguration*
usbemu_configuration_new_from_string (const gchar  *str,
                                      GError      **error)
{
  gint argc;
  gchar **argv;
  UsbemuConfiguration *configuration;

  /* g_shell_parse_argv() returns error upon empty input string, but we'd like
   * to create a generic object instead. */
  if ((str == NULL) || (*str == '\0'))
    return usbemu_configuration_new ();

  if (!g_shell_parse_argv (str, &argc, &argv, error))
    return NULL;

  configuration = usbemu_configuration_new_from_argv (argv, error);
  g_strfreev (argv);

  return configuration;
}

/**
 * usbemu_configuration_get_configuration_value:
 * @configuration: (in): the #UsbemuConfiguration object.
 *
 * Get the value identifying this configuration.
 *
 * Returns: selection value or 0 if error.
 */
guint
usbemu_configuration_get_configuration_value (UsbemuConfiguration *configuration)
{
  g_return_val_if_fail (USBEMU_IS_CONFIGURATION (configuration), 0);

  return configuration->bConfigurationValue;
}

/**
 * usbemu_configuration_get_name:
 * @configuration: (in): the #UsbemuConfiguration object.
 *
 * Get device configuration's name.
 *
 * Returns: (transfer none) : The name of this configuration object owned by
 *          itself or %NULL if error. Do not free.
 */
const gchar*
usbemu_configuration_get_name (UsbemuConfiguration *configuration)
{
  g_return_val_if_fail (USBEMU_IS_CONFIGURATION (configuration), NULL);

  return configuration->name;
}

/**
 * usbemu_configuration_set_name:
 * @configuration: (in): the #UsbemuConfiguration object.
 * @name: (in): the new name or %NULL to remove it.
 *
 * Set device configuration's name.
 */
void
usbemu_configuration_set_name (UsbemuConfiguration *configuration,
                               const gchar         *name)
{
  g_return_if_fail (USBEMU_IS_CONFIGURATION (configuration));

  g_object_set ((GObject*) configuration,
                USBEMU_CONFIGURATION_PROP_NAME, name,
                NULL);
}

/**
 * usbemu_configuration_get_attributes:
 * @configuration: (in): the #UsbemuConfiguration object.
 *
 * Get device configuration's attributes.
 *
 * Returns: attribute flags of this configuration object or 0 if error.
 */
guint
usbemu_configuration_get_attributes (UsbemuConfiguration *configuration)
{
  g_return_val_if_fail (USBEMU_IS_CONFIGURATION (configuration), 0);

  return configuration->bmAttributes;
}

/**
 * usbemu_configuration_set_attributes:
 * @configuration: (in): the #UsbemuConfiguration object.
 * @attributes: the new attributes.
 *
 * Set device configuration's attributes.
 */
void
usbemu_configuration_set_attributes (UsbemuConfiguration *configuration,
                                     guint                attributes)
{
  g_return_if_fail (USBEMU_IS_CONFIGURATION (configuration));

  g_object_set ((GObject*) configuration,
                USBEMU_CONFIGURATION_PROP_ATTRIBUTES, attributes,
                NULL);
}

/**
 * usbemu_configuration_get_max_power:
 * @configuration: (in): the #UsbemuConfiguration object.
 *
 * Get device configuration's maximum power consumption.
 *
 * Returns: maximum power consumption in mA of this configuration object or 0 if
 *          error.
 */
guint
usbemu_configuration_get_max_power (UsbemuConfiguration *configuration)
{
  g_return_val_if_fail (USBEMU_IS_CONFIGURATION (configuration), 0);

  return configuration->bMaxPower;
}

/**
 * usbemu_configuration_set_max_power:
 * @configuration: (in): the #UsbemuConfiguration object.
 * @max_power: the new maximum power consumption in mA.
 *
 * Set device configuration's maximum power consumption.
 */
void
usbemu_configuration_set_max_power (UsbemuConfiguration *configuration,
                                    guint                max_power)
{
  g_return_if_fail (USBEMU_IS_CONFIGURATION (configuration));

  g_object_set ((GObject*) configuration,
                USBEMU_CONFIGURATION_PROP_MAX_POWER, max_power,
                NULL);
}

/**
 * usbemu_configuration_get_device:
 * @configuration: (in): the #UsbemuConfiguration object.
 *
 * Get the bonded #UsbemuDevice of this configuration.
 *
 * Returns: (transfer full) (type UsbemuDevice): a #UsbemuDevice.
 */
UsbemuDevice*
usbemu_configuration_get_device (UsbemuConfiguration *configuration)
{
  g_return_val_if_fail (USBEMU_IS_CONFIGURATION (configuration), NULL);

  if (configuration->device != NULL)
    return g_object_ref (configuration->device);

  return NULL;
}

static GPtrArray*
_create_interfaces_array ()
{
  return g_ptr_array_new_with_free_func ((GDestroyNotify) g_object_unref);
}

static GPtrArray*
_get_alternate_interfaces (UsbemuConfiguration *configuration,
                           guint                interface_number)
{
  g_return_val_if_fail (USBEMU_IS_CONFIGURATION (configuration), NULL);

  if (interface_number >= configuration->interface_groups->len)
    return NULL;

  return g_ptr_array_index (configuration->interface_groups, interface_number);
}

/**
 * usbemu_configuration_get_n_interface_groups:
 * @configuration: (in): the #UsbemuConfiguration object.
 *
 * Get number of interface alternate setting groups available.
 *
 * Returns: number of interface alternate setting groups.
 */
guint
usbemu_configuration_get_n_interface_groups (UsbemuConfiguration *configuration)
{
  g_return_val_if_fail (USBEMU_IS_CONFIGURATION (configuration), 0);

  return configuration->interface_groups->len;
}

/**
 * usbemu_configuration_add_alternate_interface:
 * @configuration: (in): the #UsbemuConfiguration object.
 * @interface_number: an interface number.
 * @interface: (in): the #UsbemuInterface object.
 *
 * Add an interface as the last alternate setting of an interface group
 * specified by @interface_number. If @interface_number equals to next valid
 * interface number, a new group is created; or, if larger than that, it's
 * considered an error.
 *
 * Returns: %TRUE if added, %FALSE otherwise.
 */
gboolean
usbemu_configuration_add_alternate_interface (UsbemuConfiguration *configuration,
                                              guint                interface_number,
                                              UsbemuInterface     *interface)
{
  GPtrArray *interfaces;

  g_return_val_if_fail (USBEMU_IS_CONFIGURATION (configuration), FALSE);
  g_return_val_if_fail (USBEMU_IS_INTERFACE (interface), FALSE);

  if (interface_number > configuration->interface_groups->len)
    return FALSE;

  if (interface_number == configuration->interface_groups->len) {
    interfaces = _create_interfaces_array ();
    g_ptr_array_add (configuration->interface_groups, interfaces);
  } else {
    interfaces = g_ptr_array_index (configuration->interface_groups,
                                    interface_number);
  }

  g_ptr_array_add (interfaces, g_object_ref (interface));
  _usbemu_interface_set_configuration (interface, configuration,
                                       interface_number, interfaces->len - 1);

  return TRUE;
}

/**
 * usbemu_configuration_get_alternate_interface:
 * @configuration: (in): the #UsbemuConfiguration object.
 * @interface_number: an interface number.
 * @alternate_setting: a alternate setting value.
 *
 * Get the #UsbemuInterface object of specified interface number and alternate
 * setting value.
 *
 * Returns: (transfer full): a #UsbemuInterface object or %NULL if error.
 */
UsbemuInterface*
usbemu_configuration_get_alternate_interface (UsbemuConfiguration *configuration,
                                              guint                interface_number,
                                              guint                alternate_setting)
{
  GPtrArray *interfaces;

  interfaces = _get_alternate_interfaces (configuration, interface_number);
  if (interfaces == NULL)
    return NULL;

  if (alternate_setting >= interfaces->len)
    return NULL;

  return g_object_ref (g_ptr_array_index (interfaces, alternate_setting));
}

/**
 * usbemu_configuration_get_alternate_interfaces:
 * @configuration: (in): the #UsbemuConfiguration object.
 * @interface_number: an interface number.
 *
 * Get all interfaces with specified interface number in this configuration. Use
 * g_array_unref() to free the returned array.
 *
 * Returns: (transfer full) (type GPtrArray(UsbemuInterface*)):
 *          #UsbemuInterface objects or %NULL if not found.
 */
GPtrArray*
usbemu_configuration_get_alternate_interfaces (UsbemuConfiguration *configuration,
                                               guint                interface_number)
{
  GPtrArray *interfaces;
  GPtrArray *ret;
  gsize i;

  interfaces = _get_alternate_interfaces (configuration, interface_number);
  if (interfaces == NULL)
    return NULL;

  ret = _create_interfaces_array ();
  for (i = 0; i < interfaces->len; i++)
    g_ptr_array_add (ret, g_object_ref (g_ptr_array_index (interfaces, i)));

  return ret;
}

/**
 * usbemu_configuration_get_n_alternate_interfaces:
 * @configuration: (in): the #UsbemuConfiguration object.
 * @interface_number: an interface number.
 *
 * Get the number of available alternate interfaces of a given interface number.
 *
 * Returns: number of available interfaces.
 */
guint
usbemu_configuration_get_n_alternate_interfaces (UsbemuConfiguration *configuration,
                                                 guint                interface_number)
{
  GPtrArray *interfaces;

  interfaces = _get_alternate_interfaces (configuration, interface_number);
  return (interfaces != NULL) ? interfaces->len : 0;
}

void
_usbemu_configuration_set_device (UsbemuConfiguration *configuration,
                                  UsbemuDevice        *device,
                                  guint                configuration_value)
{
  g_assert_true ((device != NULL) && (configuration->device == NULL));

  configuration->device = g_object_ref (device);
  configuration->bConfigurationValue = configuration_value;
}
