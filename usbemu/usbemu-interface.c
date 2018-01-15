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

#include "usbemu/usbemu-configuration.h"
#include "usbemu/usbemu-device.h"
#include "usbemu/usbemu-enums.h"
#include "usbemu/usbemu-interface.h"

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
  gint bInterfaceClass;
  guint bInterfaceSubClass;
  guint bInterfaceProtocol;
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
#define USBEMU_INTERFACE_PROP_SUB_CLASS__DEFAULT 0xFF
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
  g_return_val_if_fail (USBEMU_IS_INTERFACE (interface), NULL);

  return g_object_ref (USBEMU_INTERFACE_GET_PRIVATE (interface)->configuration);
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
