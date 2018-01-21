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

#include <string.h>
#include <stdlib.h>

#include "usbemu/usbemu-configuration.h"
#include "usbemu/usbemu-device.h"
#include "usbemu/usbemu-enums.h"
#include "usbemu/usbemu-interface.h"
#include "usbemu/usbemu-internal.h"

/**
 * SECTION:usbemu-interface
 * @title: UsbemuInterface
 * @short_description: GObject representing USB device interface descriptor
 * @include: usbemu/usbemu.h
 *
 * TODO.
 */

/**
 * UsbemuInterface:
 *
 * GObject representing USB device interface descriptor.
 */

/**
 * UsbemuInterfaceClass:
 * @parent_class: The parent class.
 *
 * Class structure for UsbemuInterface.
 */

typedef struct  _UsbemuDevicePrivate {
  UsbemuConfiguration *configuration;
  guint bInterfaceNumber;
  guint bAlternateSetting;

  gchar *name;
  UsbemuClasses bInterfaceClass;
  guint bInterfaceSubClass;
  guint bInterfaceProtocol;
  UsbemuEndpointEntry endpoints[(USBEMU_NUM_ENDPOINTS - 1) * 2 + 1];
  gsize n_endpoints;
} UsbemuInterfacePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (UsbemuInterface, usbemu_interface, G_TYPE_OBJECT)

#define USBEMU_INTERFACE_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), USBEMU_TYPE_INTERFACE, UsbemuInterfacePrivate))

enum
{
  PROP_0,
  PROP_INTERFACE_NUMBER,
  PROP_ALTERNATE_SETTING,
  PROP_NAME,
  PROP_CLASS,
  PROP_SUB_CLASS,
  PROP_PROTOCOL,
  N_PROPERTIES
};

static GParamSpec *props[N_PROPERTIES] = { NULL, };

#define USBEMU_INTERFACE_PROP_INTERFACE_NUMBER__DEFAULT 0
#define USBEMU_INTERFACE_PROP_ALTERNATE_SETTING__DEFAULT 0
#define USBEMU_INTERFACE_PROP_NAME__DEFAULT NULL
#define USBEMU_INTERFACE_PROP_CLASS__DEFAULT USBEMU_CLASS_VENDOR_SPECIFIC
#define USBEMU_INTERFACE_PROP_SUB_CLASS__DEFAULT 0
#define USBEMU_INTERFACE_PROP_PROTOCOL__DEFAULT USBEMU_PROTOCOL_VENDOR_SPECIFIC

/* virtual methods for GObjectClass */
static void gobject_class_set_property (GObject *object, guint prop_id,
                                        const GValue *value, GParamSpec *pspec);
static void gobject_class_get_property (GObject *object, guint prop_id,
                                        GValue *value, GParamSpec *pspec);
static void gobject_class_dispose (GObject *object);
static void gobject_class_finalize (GObject *object);
/* virtual methods for UsbemuInterfaceClass */
static void usbemu_interface_class_init (UsbemuInterfaceClass *interface_class);

static void
gobject_class_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  UsbemuInterface *interface = USBEMU_INTERFACE (object);
  UsbemuInterfacePrivate *priv = USBEMU_INTERFACE_GET_PRIVATE (interface);

  switch (prop_id) {
    case PROP_NAME:
      if (priv->name)
        g_free (priv->name);
      priv->name = g_value_dup_string (value);
      break;
    case PROP_CLASS:
      priv->bInterfaceClass = g_value_get_enum (value);
      break;
    case PROP_SUB_CLASS:
      priv->bInterfaceSubClass = g_value_get_uint (value);
      break;
    case PROP_PROTOCOL:
      priv->bInterfaceProtocol = g_value_get_uint (value);
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
  UsbemuInterface *interface = USBEMU_INTERFACE (object);
  UsbemuInterfacePrivate *priv = USBEMU_INTERFACE_GET_PRIVATE (interface);

  switch (prop_id) {
    case PROP_INTERFACE_NUMBER:
      g_value_set_uint (value, priv->bInterfaceNumber);
      break;
    case PROP_ALTERNATE_SETTING:
      g_value_set_uint (value, priv->bAlternateSetting);
      break;
    case PROP_NAME:
      g_value_set_string (value, priv->name);
      break;
    case PROP_CLASS:
      g_value_set_enum (value, priv->bInterfaceClass);
      break;
    case PROP_SUB_CLASS:
      g_value_set_uint (value, priv->bInterfaceSubClass);
      break;
    case PROP_PROTOCOL:
      g_value_set_uint (value, priv->bInterfaceProtocol);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gobject_class_dispose (GObject *object)
{
  UsbemuInterface *interface = USBEMU_INTERFACE (object);
  UsbemuInterfacePrivate *priv = USBEMU_INTERFACE_GET_PRIVATE (interface);

  g_clear_object (&priv->configuration);
}

static void
gobject_class_finalize (GObject *object)
{
  UsbemuInterface *interface = USBEMU_INTERFACE (object);
  UsbemuInterfacePrivate *priv = USBEMU_INTERFACE_GET_PRIVATE (interface);

  if (priv->name)
    g_free (priv->name);
}

static void
usbemu_interface_class_init (UsbemuInterfaceClass *interface_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (interface_class);

  /* virtual methods */

  object_class->set_property = gobject_class_set_property;
  object_class->get_property = gobject_class_get_property;
  object_class->dispose = gobject_class_dispose;
  object_class->finalize = gobject_class_finalize;

  /* properties */

  /**
   * UsbemuInterface:interface-number:
   *
   * Zero-based value identifying the index in the array of concurrent
   * interfaces supported by this configuration. It's automatically assigned
   * when added to a configuration.
   */
  props[PROP_INTERFACE_NUMBER] =
        g_param_spec_uint (USBEMU_INTERFACE_PROP_INTERFACE_NUMBER,
                           "Interface Number", "Interface Number",
                           0, G_MAXUINT,
                           USBEMU_INTERFACE_PROP_INTERFACE_NUMBER__DEFAULT,
                           G_PARAM_READABLE);

  /**
   * UsbemuInterface:alternate-setting:
   *
   * The value used to select this alternate setting from the interfaces
   * identified with the same interface number. It's automatically assigned
   * when added to a configuration.
   */
  props[PROP_ALTERNATE_SETTING] =
        g_param_spec_uint (USBEMU_INTERFACE_PROP_ALTERNATE_SETTING,
                           "Alternate Setting", "Alternate Setting",
                           0, G_MAXUINT,
                           USBEMU_INTERFACE_PROP_ALTERNATE_SETTING__DEFAULT,
                           G_PARAM_READABLE);

  /**
   * UsbemuInterface:name: (nullable)
   */
  props[PROP_NAME] =
        g_param_spec_string (USBEMU_INTERFACE_PROP_NAME,
                             "Name", "Name",
                             USBEMU_INTERFACE_PROP_NAME__DEFAULT,
                             G_PARAM_READWRITE | \
                               G_PARAM_CONSTRUCT);

  /**
   * UsbemuInterface:class:
   *
   * Class code (assigned by the
   * [USB-IF](http://www.usb.org/developers/defined_class)).
   *
   * A value of zero is reserved for future standardization. If this field is
   * set to #USBEMU_CLASS_VENDOR_SPECIFIC, the interface class is
   * vendor-specific. All other values are reserved for assignment by the
   * USB-IF.
   */
  props[PROP_CLASS] =
        g_param_spec_enum (USBEMU_INTERFACE_PROP_CLASS,
                            "Class", "Class",
                            USBEMU_TYPE_CLASSES,
                            USBEMU_INTERFACE_PROP_CLASS__DEFAULT,
                            G_PARAM_READWRITE | \
                              G_PARAM_CONSTRUCT);

  /**
   * UsbemuInterface:sub-class:
   *
   * Subclass code (assigned by the
   * [USB-IF](http://www.usb.org/developers/defined_class)).
   *
   * These codes are qualified by the value of the #UsbemuInterface:class field.
   * If the bInterfaceClass field is reset to
   * #USBEMU_CLASS_USE_INTERFACE_DESCRIPTOR, this field must also be reset to
   * #USBEMU_SUB_CLASS_USE_INTERFACE_DESCRIPTOR. If the #UsbemuInterface:class
   * field is not set to #USBEMU_CLASS_VENDOR_SPECIFIC, all values are reserved
   * for assignment by the USB-IF.
   */
  props[PROP_SUB_CLASS] =
        g_param_spec_uint (USBEMU_INTERFACE_PROP_SUB_CLASS,
                           "Sub Class", "Sub Class",
                           0, G_MAXUINT,
                           USBEMU_INTERFACE_PROP_SUB_CLASS__DEFAULT,
                           G_PARAM_READWRITE | \
                             G_PARAM_CONSTRUCT);

  /**
   * UsbemuInterface:protocol:
   *
   *  Protocol code (assigned by the
   * [USB-IF](http://www.usb.org/developers/defined_class)).
   *
   * These codes are qualified by the value of the #UsbemuInterface:class and
   * the #UsbemuInterface:sub-class fields. If an interface supports
   * class-specific requests, this code identifies the protocols that the device
   * uses as defined by the specification of the device class.
   *
   * If this field is reset to zero, the device does not use a class-specific
   * protocol on this interface.
   *
   * If this field is set to #USBEMU_PROTOCOL_VENDOR_SPECIFIC, the device uses
   * a vendor-specific protocol for this interface.
   */
  props[PROP_PROTOCOL] =
        g_param_spec_uint (USBEMU_INTERFACE_PROP_PROTOCOL,
                           "Protocol", "Protocol",
                           0, G_MAXUINT,
                           USBEMU_INTERFACE_PROP_PROTOCOL__DEFAULT,
                           G_PARAM_READWRITE | \
                             G_PARAM_CONSTRUCT);

  g_object_class_install_properties (object_class, N_PROPERTIES, props);
}

static void
usbemu_interface_init (UsbemuInterface *interface)
{
  UsbemuInterfacePrivate *priv = USBEMU_INTERFACE_GET_PRIVATE (interface);

  priv->bInterfaceNumber = USBEMU_INTERFACE_PROP_INTERFACE_NUMBER__DEFAULT;
  priv->bAlternateSetting = USBEMU_INTERFACE_PROP_ALTERNATE_SETTING__DEFAULT;
  priv->name = USBEMU_INTERFACE_PROP_NAME__DEFAULT;
  priv->bInterfaceClass = USBEMU_INTERFACE_PROP_CLASS__DEFAULT;
  priv->bInterfaceSubClass = USBEMU_INTERFACE_PROP_SUB_CLASS__DEFAULT;
  priv->bInterfaceProtocol = USBEMU_INTERFACE_PROP_PROTOCOL__DEFAULT;
  priv->configuration = NULL;
  memset (priv->endpoints, 0, sizeof (priv->endpoints));
  priv->n_endpoints = 0;
}

/**
 * usbemu_interface_new:
 *
 * Create a new #UsbemuInterface instance.
 *
 * Returns: (transfer full) (type UsbemuInterface): The constructed device
 *          interface object or %NULL.
 */
UsbemuInterface*
usbemu_interface_new ()
{
  return g_object_new (USBEMU_TYPE_INTERFACE, NULL);
}

/**
 * usbemu_interface_new_full:
 * @name: (in): a %NULL-terminated name or %NULL to remove it.
 * @klass: the interface class.
 * @sub_class: the interface sub-class.
 * @protocol: the interface protocol.
 *
 * Create a new #UsbemuInterface instance with details.
 *
 * Returns: (transfer full) (type UsbemuInterface): The constructed device
 *          interface object or %NULL.
 */
UsbemuInterface*
usbemu_interface_new_full (const gchar   *name,
                           UsbemuClasses  klass,
                           guint          sub_class,
                           guint          protocol)
{
  return g_object_new (USBEMU_TYPE_INTERFACE,
                       USBEMU_INTERFACE_PROP_NAME, name,
                       USBEMU_INTERFACE_PROP_CLASS, klass,
                       USBEMU_INTERFACE_PROP_SUB_CLASS, sub_class,
                       USBEMU_INTERFACE_PROP_PROTOCOL, protocol,
                       NULL);
}

static gboolean
_decode_endpoint_attributes_value (gchar *value,
                                   guint8 *attributes)
{
  GEnumValue *enum_value;
  gchar *p;
  gint sync = 0, usage = 0;

  g_assert_nonnull (value);
  g_assert_nonnull (attributes);

  do {
    p = strchr (value, '|');
    if (p != NULL) {
      *p = '\0';
      p++;
    }

    enum_value = usbemu_enum_get_value_by_name_or_nick (
          USBEMU_TYPE_ENDPOINT_ISOCHRONOUS_SYNCS, value);
    if (enum_value != NULL)
      sync = enum_value->value;
    else {
      enum_value = usbemu_enum_get_value_by_name_or_nick (
            USBEMU_TYPE_ENDPOINT_ISOCHRONOUS_USAGES, value);
      if (enum_value != NULL)
        usage = enum_value->value;
      else
        return FALSE;
    }

    value = p;
  } while (value != NULL);

  *attributes = (sync | usage);
  return TRUE;
}

static gboolean
_extract_endpoints_from_argv (gchar               ***argv,
                              UsbemuEndpointEntry   *entry,
                              GError               **error)
{
  gchar **strv, *k, *v;
  gboolean ret = TRUE;
  GEnumValue *enum_value;

  g_assert_nonnull (argv);
  g_assert_nonnull (entry);

  strv = *argv;
  while ((k = *strv) != NULL) {
    if (*k == '-')
      break;

    ++strv;

    v = strchr (k, '=');
    if ((v == NULL) || (*(v + 1) == '\0') || (v == k)) {
      ret = FALSE;
      break;
    }

    *v = '\0'; ++v;

#define DECODE_ENUM_FIELD(name, type) \
    (g_ascii_strcasecmp (k, #name) == 0) { \
      enum_value = usbemu_enum_get_value_by_name_or_nick (type, v); \
      if (enum_value != NULL) { \
        entry->name = enum_value->value; \
        continue; \
      } \
    }

    if DECODE_ENUM_FIELD (endpoint_number, USBEMU_TYPE_ENDPOINTS)
    else if DECODE_ENUM_FIELD (direction, USBEMU_TYPE_ENDPOINT_DIRECTIONS)
    else if DECODE_ENUM_FIELD (transfer, USBEMU_TYPE_ENDPOINT_TRANSFERS)
    else if (g_ascii_strcasecmp (k, "attributes") == 0) {
      if (_decode_endpoint_attributes_value (v, &entry->attributes))
        continue;
    } else if (g_ascii_strcasecmp (k, "max_packet_size") == 0) {
      entry->max_packet_size = strtoul (v, NULL, 0);
      continue;
    } else if (g_ascii_strcasecmp (k, "additional_transactions") == 0) {
      entry->additional_transactions = strtoul (v, NULL, 0);
      continue;
    } else if (g_ascii_strcasecmp (k, "interval") == 0) {
      entry->interval = strtoul (v, NULL, 0);
      continue;
    }

#undef DECODE_ENUM_FIELD

    ret = FALSE;
    break;
  }
  *argv = strv;

  if (!ret) {
    g_set_error (error, USBEMU_ERROR, USBEMU_ERROR_SYNTAX_ERROR,
                 "syntax error at '%s'", k);
  }

  return ret;
}

UsbemuInterface*
_usbemu_interface_new_from_argv_inner (gchar    ***argv,
                                       GError    **error,
                                       gboolean    allow_remaining)
{
  GType interface_type;
  UsbemuInterface *interface;
  gchar **strv;
  GArray *endpoints;
  UsbemuEndpointEntry *entry;

  interface_type = USBEMU_TYPE_INTERFACE;
  interface = (UsbemuInterface*)
        _usbemu_object_new_from_argv (argv, &interface_type, "object-type",
                                      error);
  if (interface == NULL)
    return NULL;

  if (*argv == NULL)
    return interface;

  endpoints = g_array_new (FALSE, TRUE, sizeof (UsbemuEndpointEntry));

  strv = *argv;
  while (*strv != NULL) {
    if (g_ascii_strcasecmp (*strv, "--endpoint") == 0) {
      ++strv;

      g_array_set_size (endpoints, endpoints->len + 1);
      entry = &g_array_index (endpoints,
                              UsbemuEndpointEntry, endpoints->len - 1);
      if (_extract_endpoints_from_argv (&strv, entry, error))
        continue;
    } else if (allow_remaining && (**strv == '-')) {
      break;
    } else {
      g_set_error (error, USBEMU_ERROR, USBEMU_ERROR_SYNTAX_ERROR,
                   "unknown argument '%s'", *strv);
    }

    g_object_unref (interface);
    interface = NULL;
    break;
  }
  *argv = strv;

  if ((interface != NULL) && (endpoints->len > 0))
    usbemu_interface_add_endpoint_entries (interface,
                                           (const UsbemuEndpointEntry*) endpoints->data);

  g_array_free (endpoints, TRUE);

  return interface;
}

/**
 * usbemu_interface_new_from_argv:
 * @argv: (in) (optional) (array zero-terminated=1): interface description.
 * @error: (out) (optional): return location for error.
 *
 * Create #UsbemuInterface from tokenized command line string. See
 * usbemu_device_new_from_string() for valid syntax.
 *
 * A generic interface object is created when %NULL or an empty strv is
 * passed.
 *
 * Returns: (transfer full): a newly created #UsbemuInterface object or
 *          %NULL if failed.
 */
UsbemuInterface*
usbemu_interface_new_from_argv (gchar  **argv,
                                GError **error)
{
  return _usbemu_interface_new_from_argv_inner (&argv, error, FALSE);
}

/**
 * usbemu_interface_new_from_string:
 * @str: (in) (optional): command line like interface description.
 * @error: (out) (optional): return location for error.
 *
 * Create #UsbemuInterface from a command line like formated string. See
 * usbemu_device_new_from_string() for valid syntax.
 *
 * A generic interface object is created when %NULL or an empty string is
 * passed.
 *
 * Returns: (transfer full): a newly created #UsbemuInterface object or %NULL if
 *          failed.
 */
UsbemuInterface*
usbemu_interface_new_from_string (const gchar  *str,
                                  GError      **error)
{
  gint argc;
  gchar **argv;
  UsbemuInterface *interface;

  /* g_shell_parse_argv() returns error upon empty input string, but we'd like
   * to create a generic object instead. */
  if ((str == NULL) || (*str == '\0'))
    return usbemu_interface_new ();

  if (!g_shell_parse_argv (str, &argc, &argv, error))
    return NULL;

  interface = usbemu_interface_new_from_argv (argv, error);
  g_strfreev (argv);

  return interface;
}

/**
 * usbemu_interface_get_interface_number:
 * @interface: (in): a #UsbemuInterface object.
 *
 * Get number of this interface.
 *
 * Returns: interface number or 0 if not assigned yet.
 */
guint
usbemu_interface_get_interface_number (UsbemuInterface *interface)
{
  g_return_val_if_fail (USBEMU_IS_INTERFACE (interface), 0);

  return USBEMU_INTERFACE_GET_PRIVATE (interface)->bInterfaceNumber;
}

/**
 * usbemu_interface_get_alternate_setting:
 * @interface: (in): a #UsbemuInterface object.
 *
 * Get interface alternate setting number.
 *
 * Returns: alternate setting number or 0 otherwise.
 */
guint
usbemu_interface_get_alternate_setting (UsbemuInterface *interface)
{
  g_return_val_if_fail (USBEMU_IS_INTERFACE (interface), 0);

  return USBEMU_INTERFACE_GET_PRIVATE (interface)->bAlternateSetting;
}

/**
 * usbemu_interface_get_name:
 * @interface: (in): a #UsbemuInterface object.
 *
 * Get interface name.
 *
 * Returns: (transfer none) (nullable): interface name.
 */
const gchar*
usbemu_interface_get_name (UsbemuInterface *interface)
{
  g_return_val_if_fail (USBEMU_IS_INTERFACE (interface), NULL);

  return USBEMU_INTERFACE_GET_PRIVATE (interface)->name;
}

/**
 * usbemu_interface_set_name:
 * @interface: (in): a #UsbemuInterface object.
 * @name: (in) (nullable): a non-%NULL string name or %NULL to remove it.
 *
 * Set interface name.
 */
void
usbemu_interface_set_name (UsbemuInterface *interface,
                           const gchar     *name)
{
  g_return_if_fail (USBEMU_IS_INTERFACE (interface));

  UsbemuInterfacePrivate *priv = USBEMU_INTERFACE_GET_PRIVATE (interface);
  if (priv->name != NULL)
    g_free (priv->name);
  priv->name = g_strdup (name);
}

/**
 * usbemu_interface_get_class:
 * @interface: (in): a #UsbemuInterface object.
 *
 * Get interface class code.
 *
 * Returns: a #UsbemuClasses.
 */
UsbemuClasses
usbemu_interface_get_class (UsbemuInterface *interface)
{
  g_return_val_if_fail (USBEMU_IS_INTERFACE (interface),
                        USBEMU_CLASS_USE_INTERFACE_DESCRIPTOR);

  return USBEMU_INTERFACE_GET_PRIVATE (interface)->bInterfaceClass;
}

/**
 * usbemu_interface_set_class:
 * @interface: (in): a #UsbemuInterface object.
 * @klass: a #UsbemuClasses value.
 *
 * Set interface class code.
 */
void
usbemu_interface_set_class (UsbemuInterface *interface,
                            UsbemuClasses    klass)
{
  g_return_if_fail (USBEMU_IS_INTERFACE (interface));

  USBEMU_INTERFACE_GET_PRIVATE (interface)->bInterfaceClass = klass;
}

/**
 * usbemu_interface_get_sub_class:
 * @interface: (in): a #UsbemuInterface object.
 *
 * Get interface sub-class code.
 *
 * Returns: interface sub-class code.
 */
guint
usbemu_interface_get_sub_class (UsbemuInterface *interface)
{
  g_return_val_if_fail (USBEMU_IS_INTERFACE (interface),
                        USBEMU_SUB_CLASS_USE_INTERFACE_DESCRIPTOR);

  return USBEMU_INTERFACE_GET_PRIVATE (interface)->bInterfaceSubClass;
}

/**
 * usbemu_interface_set_sub_class:
 * @interface: (in): a #UsbemuInterface object.
 * @sub_class: a sub-class code.
 *
 * Set interface sub-class code.
 */
void
usbemu_interface_set_sub_class (UsbemuInterface *interface,
                                guint            sub_class)
{
  g_return_if_fail (USBEMU_IS_INTERFACE (interface));

  USBEMU_INTERFACE_GET_PRIVATE (interface)->bInterfaceSubClass = sub_class;
}

/**
 * usbemu_interface_get_protocol:
 * @interface: (in): a #UsbemuInterface object.
 *
 * Get interface protocol code.
 *
 * Returns: interface protocol code.
 */
guint
usbemu_interface_get_protocol (UsbemuInterface *interface)
{
  g_return_val_if_fail (USBEMU_IS_INTERFACE (interface),
                        USBEMU_PROTOCOL_USE_INTERFACE_DESCRIPTOR);

  return USBEMU_INTERFACE_GET_PRIVATE (interface)->bInterfaceProtocol;
}

/**
 * usbemu_interface_set_protocol:
 * @interface: (in): a #UsbemuInterface object.
 * @protocol: a interface protocol code.
 *
 * Set interface protocol code.
 */
void
usbemu_interface_set_protocol (UsbemuInterface *interface,
                               guint            protocol)
{
  g_return_if_fail (USBEMU_IS_INTERFACE (interface));

  USBEMU_INTERFACE_GET_PRIVATE (interface)->bInterfaceProtocol = protocol;
}

/**
 * usbemu_interface_get_configuration:
 * @interface: (in): a #UsbemuInterface object.
 *
 * Get the #UsbemuConfiguration this interface was added to.
 *
 * Returns: (transfer full): a #UsbemuConfiguration or %NULL if not added to any
 *          yet.
 */
UsbemuConfiguration*
usbemu_interface_get_configuration (UsbemuInterface *interface)
{
  UsbemuConfiguration *configuration;

  g_return_val_if_fail (USBEMU_IS_INTERFACE (interface), NULL);

  configuration = USBEMU_INTERFACE_GET_PRIVATE (interface)->configuration;
  if (configuration != NULL)
    return g_object_ref (configuration);

  return NULL;
}

/**
 * usbemu_interface_add_endpoint_entries:
 * @interface: a #UsbemuInterface object.
 * @entries: a %NULL-terminated array of #UsbemuEndpointEntry.
 *
 * Add endpoints to interface.
 */
void
usbemu_interface_add_endpoint_entries (UsbemuInterface           *interface,
                                       const UsbemuEndpointEntry *entries)
{
  gsize n_entries;
  UsbemuInterfacePrivate *priv;
  gboolean valid = TRUE;

  g_return_if_fail (USBEMU_IS_INTERFACE (interface));
  g_return_if_fail (entries != NULL);

  for (n_entries = 0; entries[n_entries].endpoint_number; n_entries++);

  priv = USBEMU_INTERFACE_GET_PRIVATE (interface);
  if ((n_entries + priv->n_endpoints) >= G_N_ELEMENTS (priv->endpoints))
    return;

  memcpy (&priv->endpoints[priv->n_endpoints], entries,
          n_entries * sizeof (priv->endpoints[0]));
  n_entries += priv->n_endpoints;

  /* FIXME: checks here */

  if (valid)
    priv->n_endpoints = n_entries;
  else
    priv->endpoints[priv->n_endpoints].endpoint_number = 0;
}

/**
 * usbemu_interface_get_endpoint_entries:
 * @interface: a #UsbemuInterface object.
 *
 * Get all #UsbemuEndpointEntry entries added to this interface.
 *
 * Returns: (transfer none) (array zero-terminated=1): %NULL-terminated array of
 *          #UsbemuEndpointEntry added to this interface.
 */
const UsbemuEndpointEntry*
usbemu_interface_get_endpoint_entries (UsbemuInterface *interface)
{
  g_return_val_if_fail (USBEMU_IS_INTERFACE (interface), NULL);

  return USBEMU_INTERFACE_GET_PRIVATE (interface)->endpoints;
}

void
_usbemu_interface_set_configuration (UsbemuInterface     *interface,
                                     UsbemuConfiguration *configuration,
                                     guint                interface_number,
                                     guint                alternate_setting)
{
  UsbemuInterfacePrivate *priv = USBEMU_INTERFACE_GET_PRIVATE (interface);

  priv->configuration = g_object_ref (configuration);
  priv->bInterfaceNumber = interface_number;
  priv->bAlternateSetting = alternate_setting;
}
