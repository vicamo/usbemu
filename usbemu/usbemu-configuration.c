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
#include "usbemu/usbemu-internal.h"

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
  GSList *interfaces;
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
/* helper functions */
static void _free_interfaces_slist_inner (GSList *slist);

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
_free_interfaces_slist_inner (GSList *slist)
{
  g_slist_free_full (slist, (GDestroyNotify) g_object_unref);
}

static void
gobject_class_dispose (GObject *object)
{
  UsbemuConfiguration *configuration = USBEMU_CONFIGURATION (object);

  if (configuration->interfaces != NULL) {
    g_slist_free_full (configuration->interfaces,
                       (GDestroyNotify) _free_interfaces_slist_inner);
    configuration->interfaces = NULL;
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
  configuration->interfaces = NULL;
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
  GArray *interfaces;

  configuration_type = USBEMU_TYPE_CONFIGURATION;
  configuration = (UsbemuConfiguration*)
        _usbemu_object_new_from_argv (argv, &configuration_type, NULL, error);
  if (configuration == NULL)
    return NULL;

  interfaces = g_array_new (TRUE, TRUE, sizeof (UsbemuInterface*));
  g_array_set_clear_func (interfaces,
                          (GDestroyNotify) _usbemu_object_unref_dereferenced);

  strv = *argv;
  while (*strv != NULL) {
    if (g_ascii_strcasecmp (*strv, "--interface") == 0) {
      ++strv;

      if (interfaces->len) {
        usbemu_configuration_add_alternate_interfaces (configuration,
                                                       (UsbemuInterface**) interfaces->data);
        g_array_remove_range (interfaces, 0, interfaces->len);
      }
      continue;
    } else if (g_ascii_strcasecmp (*strv, "--alternate-setting") == 0) {
      UsbemuInterface *interface;

      ++strv;
      interface = _usbemu_interface_new_from_argv_inner (&strv, error, TRUE);
      if (interface != NULL) {
        g_array_append_val (interfaces, interface);
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

  if ((configuration != NULL) && interfaces->len) {
    usbemu_configuration_add_alternate_interfaces (configuration,
                                                   (UsbemuInterface**) interfaces->data);
  }

  g_array_free (interfaces, TRUE);

  return configuration;
}

/**
 * usbemu_configuration_new_from_argv:
 * @argv: (in) (array zero-terminated=1): configuration description.
 * @error: (out) (optional): return location for error.
 *
 * Create #UsbemuConfiguration from tokenized command line string. See
 * usbemu_device_new_from_string() for valid syntax.
 *
 * Returns: (transfer full): a newly created #UsbemuConfiguration object or
 *          %NULL if failed.
 */
UsbemuConfiguration*
usbemu_configuration_new_from_argv (gchar  **argv,
                                    GError **error)
{
  g_return_val_if_fail ((argv != NULL), NULL);

  return _usbemu_configuration_new_from_argv_inner (&argv, error, FALSE);
}

/**
 * usbemu_configuration_new_from_string:
 * @str: (in): command line like configuration description.
 * @error: (out) (optional): return location for error.
 *
 * Create #UsbemuConfiguration from a command line like formated string. See
 * usbemu_device_new_from_string() for valid syntax.
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

  g_return_val_if_fail ((str != NULL), NULL);

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

/**
 * usbemu_configuration_add_alternate_interfaces:
 * @configuration: (in): the #UsbemuConfiguration object.
 * @interfaces: (in) (array zero-terminated=1) (transfer none): a
 *     %NULL-terminated array of #UsbemuInterface objects.
 *
 * Add an multiple interfaces to configuration. These interfaces, if valid, will
 * be assigned with a new, identical interface number, and each of them an
 * increamental alternate setting number.
 *
 * Returns: interface number of added interfaces if succeeded. -1 otherwise.
 */
gint
usbemu_configuration_add_alternate_interfaces (UsbemuConfiguration  *configuration,
                                               UsbemuInterface     **interfaces)
{
  UsbemuInterface **interface;
  guint interface_number, alternate_setting;
  GSList *slist;

  g_return_val_if_fail (USBEMU_IS_CONFIGURATION (configuration), -1);
  g_return_val_if_fail ((interfaces != NULL), -1);

  for (interface = interfaces; *interface != NULL; ++interface) {
    if (usbemu_interface_get_configuration (*interface) != NULL)
      return -1;
  }

  interface_number = g_slist_length (configuration->interfaces);
  slist = NULL;
  for (interface = interfaces; *interface != NULL; ++interface) {
    slist = g_slist_append (slist, g_object_ref (*interface));
  }
  configuration->interfaces = g_slist_append (configuration->interfaces, slist);

  alternate_setting = 0;
  for (interface = interfaces; *interface != NULL;
       ++interface, ++alternate_setting) {
    _usbemu_interface_set_configuration (*interface, configuration,
                                         interface_number, alternate_setting);
  }

  return interface_number;
}

/**
 * usbemu_configuration_get_alternate_interfaces:
 * @configuration: (in): the #UsbemuConfiguration object.
 * @interface_number: a interface number.
 *
 * Get all interfaces with specified interface number in this configuration.
 *
 * Returns: (transfer full) (type GSList(UsbemuInterface)):
 *          #UsbemuInterface objects or %NULL if not found.
 */
GSList*
usbemu_configuration_get_alternate_interfaces (UsbemuConfiguration *configuration,
                                               guint                interface_number)
{
  GSList *slist;

  g_return_val_if_fail (USBEMU_IS_CONFIGURATION (configuration), NULL);

  slist = g_slist_nth_data (configuration->interfaces, interface_number);
  if (slist != NULL)
    slist = g_slist_copy_deep (slist, (GCopyFunc) g_object_ref, NULL);

  return slist;
}

/**
 * usbemu_configuration_get_n_alternate_interfaces:
 * @configuration: (in): the #UsbemuConfiguration object.
 *
 * Get the number of available interfaces set.
 *
 * Returns: number of available interfaces set.
 */
guint
usbemu_configuration_get_n_alternate_interfaces (UsbemuConfiguration *configuration)
{
  g_return_val_if_fail (USBEMU_IS_CONFIGURATION (configuration), 0);

  return g_slist_length (configuration->interfaces);
}

void
_usbemu_configuration_set_device (UsbemuConfiguration *configuration,
                                  UsbemuDevice        *device,
                                  guint                configuration_value)
{
  configuration->device = g_object_ref (device);
  configuration->bConfigurationValue = configuration_value;
}
