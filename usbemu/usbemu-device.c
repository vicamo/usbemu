/* usbemu - USB Emulation Library
 * Copyright (C) 2016 You-Sheng Yang
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
#include <gio/gio.h>

#include "usbemu/usbemu-device.h"
#include "usbemu/usbemu-configuration.h"
#include "usbemu/usbemu-errors.h"

#include "usbemu/internal/types.h"
#include "usbemu/internal/utils.h"

/**
 * SECTION:usbemu
 * @short_description: USB device emulation library
 * @include: usbemu/usbemu.h
 *
 * USBEmu aims to provide #GObject-based API for USB device emulation initially
 * based on function provided by Linux kernel vhci-hcd module.
 */

/**
 * SECTION:usbemu-device
 * @short_description: USB device emulation class
 * @include: usbemu/usbemu.h
 *
 * #UsbemuDevice is the core class of USB device emulation.
 */

/**
 * UsbemuDevice:
 *
 * An object representing emulated USB device.
 */

/**
 * UsbemuDeviceClass:
 * @parent_class: The parent class.
 * @attached: attached signal hook.
 * @detached: detached signal hook.
 * @attach_async: virtual method for asynchronous device attach operation.
 * @detach_async: virtual method for asynchronous device detach operation.
 *
 * Class structure for UsbemuDevice.
 */

typedef struct  _UsbemuDevicePrivate {
  gboolean attached;

  guint16 bcdUSB;
  UsbemuClasses bDeviceClass;
  guint8 bDeviceSubClass;
  guint8 bDeviceProtocol;
  guint8 bMaxPacketSize;
  guint16 idVendor;
  guint16 idProduct;
  guint16 bcdDevice;
  gchar *manufacturer;
  gchar *product;
  gchar *serial;
  UsbemuSpeeds speed;
  GPtrArray *configurations;
  GList *urb_queue;
} UsbemuDevicePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (UsbemuDevice, usbemu_device, G_TYPE_OBJECT)

#define USBEMU_DEVICE_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), USBEMU_TYPE_DEVICE, UsbemuDevicePrivate))

enum
{
  PROP_0,
  PROP_ATTACHED,
  N_PROPERTIES
};

static GParamSpec *props[N_PROPERTIES] = { NULL, };

enum
{
  SIGNAL_ATTACHED,
  SIGNAL_DETACHED,
  N_SIGNALS
};

static guint signals[N_SIGNALS] = { 0 };

/* virtual methods for GObjectClass */
static void gobject_class_set_property (GObject *object, guint prop_id,
                                        const GValue *value, GParamSpec *pspec);
static void gobject_class_get_property (GObject *object, guint prop_id,
                                        GValue *value, GParamSpec *pspec);
static void gobject_class_dispose (GObject *object);
static void gobject_class_finalize (GObject *object);
/* virtual methods for UsbemuDeviceClass */
static void usbemu_device_class_init (UsbemuDeviceClass *device_class);

static void
gobject_class_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value G_GNUC_UNUSED,
                            GParamSpec   *pspec)
{
  switch (prop_id) {
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
  UsbemuDevice *device = USBEMU_DEVICE (object);
  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);

  switch (prop_id) {
    case PROP_ATTACHED:
      g_value_set_boolean (value, priv->attached);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gobject_class_dispose (GObject *object)
{
  UsbemuDevice *device = USBEMU_DEVICE (object);
  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);

  if (priv->configurations != NULL) {
    g_ptr_array_unref (priv->configurations);
    priv->configurations = NULL;
  }
}

static void
gobject_class_finalize (GObject *object)
{
  UsbemuDevice *device = USBEMU_DEVICE (object);
  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);

  if (priv->manufacturer != NULL)
    g_free (priv->manufacturer);
  if (priv->product != NULL)
    g_free (priv->product);
  if (priv->serial != NULL)
    g_free (priv->serial);
}

static void
usbemu_device_class_init (UsbemuDeviceClass *device_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (device_class);

  /* virtual methods */

  object_class->set_property = gobject_class_set_property;
  object_class->get_property = gobject_class_get_property;
  object_class->dispose = gobject_class_dispose;
  object_class->finalize = gobject_class_finalize;

  /* signals */

  /**
   * UsbemuDevice::attached
   * @device: the device that emitted the signal
   *
   * Signals that a device has been attached.
   */
  signals[SIGNAL_ATTACHED] =
        g_signal_new (USBEMU_DEVICE_SIGNAL_ATTACHED,
                      G_TYPE_FROM_CLASS (device_class),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (UsbemuDeviceClass, attached),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

  /**
   * UsbemuDevice::detached
   * @device: the device that emitted the signal
   *
   * Signals that a device has been detached.
   */
  signals[SIGNAL_DETACHED] =
        g_signal_new (USBEMU_DEVICE_SIGNAL_DETACHED,
                      G_TYPE_FROM_CLASS (device_class),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (UsbemuDeviceClass, detached),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

  /* properties */

  /**
   * UsbemuDevice:attached:
   */
  props[PROP_ATTACHED] =
    g_param_spec_boolean (USBEMU_DEVICE_PROP_ATTACHED,
                          "Attached", "Attached",
                          FALSE,
                          G_PARAM_READABLE);

  g_object_class_install_properties (object_class, N_PROPERTIES, props);
}

static GPtrArray*
_create_configurations_array ()
{
  return g_ptr_array_new_with_free_func ((GDestroyNotify) g_object_unref);
}

static void
usbemu_device_init (UsbemuDevice *device)
{
  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);

  priv->attached = FALSE;
  priv->bcdUSB = 0x100;
  priv->bDeviceClass = USBEMU_CLASS_USE_INTERFACE_DESCRIPTOR;
  priv->bDeviceSubClass = USBEMU_SUB_CLASS_USE_INTERFACE_DESCRIPTOR;
  priv->bDeviceProtocol = USBEMU_PROTOCOL_USE_INTERFACE_DESCRIPTOR;
  priv->bMaxPacketSize = 0;
  priv->idVendor = 0xdead;
  priv->idProduct = 0xbeef;
  priv->bcdDevice = 0x100;
  priv->manufacturer = g_strdup (PACKAGE_NAME);
  priv->product = g_strdup ("emulated device");
  /* `echo -n dead:beef | md5sum` */
  priv->serial = g_strdup ("9641c4a0c0d26686a3fcdc92711f8f42");
  priv->speed = USBEMU_SPEED_UNKNOWN;
  priv->configurations = _create_configurations_array ();
}

/**
 * usbemu_device_new:
 *
 * Create a new #UsbemuDevice instance.
 *
 * Returns: (transfer full) (type UsbemuDevice): The constructed device object
 *          or %NULL.
 */
UsbemuDevice*
usbemu_device_new ()
{
  return g_object_new (USBEMU_TYPE_DEVICE, NULL);
}

UsbemuDevice*
_usbemu_device_new_from_argv_inner (gchar    ***argv,
                                    GError    **error,
                                    gboolean    allow_remaining)
{
  GType device_type;
  UsbemuDevice *device;
  gchar **strv;

  device_type = USBEMU_TYPE_DEVICE;
  device = (UsbemuDevice*) _usbemu_object_new_from_argv (argv, &device_type,
                                                         "object-type", error);
  if (device == NULL)
    return NULL;

  if (*argv == NULL)
    return device;

  strv = *argv;
  while (*strv != NULL) {
    if (g_ascii_strcasecmp (*strv, "--configuration") == 0) {
      UsbemuConfiguration *configuration;

      ++strv;
      configuration =
            _usbemu_configuration_new_from_argv_inner (&strv, error, TRUE);
      if (configuration != NULL) {
        usbemu_device_add_configuration (device, configuration);
        g_object_unref (configuration);
        continue;
      }
    } else if (allow_remaining && (**strv == '-')) {
      break;
    } else {
      g_set_error (error, USBEMU_ERROR, USBEMU_ERROR_SYNTAX_ERROR,
                   "unknown argument %s", *strv);
    }

    g_object_unref (device);
    device = NULL;
    break;
  }
  *argv = strv;

  return device;
}

/**
 * usbemu_device_new_from_argv:
 * @argv: (in) (optional) (array zero-terminated=1): device description.
 * @error: (out) (optional): return location for error.
 *
 * Create #UsbemuDevice from tokenized command line string. See
 * usbemu_device_new_from_string() for valid syntax.
 *
 * A generic interface object is created when %NULL or an empty strv is
 * passed.
 *
 * Returns: (transfer full): a newly created #UsbemuDevice object or %NULL if
 *          failed.
 */
UsbemuDevice*
usbemu_device_new_from_argv (gchar  **argv,
                             GError **error)
{
  return _usbemu_device_new_from_argv_inner (&argv, error, FALSE);
}

/**
 * usbemu_device_new_from_string:
 * @str: (in) (optional): command line like device description.
 * @error: (out) (optional): return location for error.
 *
 * Create #UsbemuDevice from a command line like formated string. Valid syntax
 * is:
 * |[
 * object-type=<device-type> [[<property-name>=<property-value>]...] \
 *   [[--configuration [[<property-name>=<property-value>]...] \
 *     [[--interface \
 *       [[--alternate-setting object-type=<interface-type> [[<property-name>=<property-value>]...] \
 *         [[--endpoint [[<field-name>=<field-value>]...]]...]
 *        ]...]
 *      ]...]
 *    ]..]
 * ]|
 *
 * A generic device object is created when %NULL or an empty string is passed.
 *
 * Returns: (transfer full): a newly created #UsbemuDevice object or %NULL if
 *          failed.
 */
UsbemuDevice*
usbemu_device_new_from_string (const gchar  *str,
                               GError      **error)
{
  gint argc;
  gchar **argv;
  UsbemuDevice *device;

  /* g_shell_parse_argv() returns error upon empty input string, but we'd like
   * to create a generic object instead. */
  if ((str == NULL) || (*str == '\0'))
    return usbemu_device_new ();

  if (!g_shell_parse_argv (str, &argc, &argv, error))
    return NULL;

  device = usbemu_device_new_from_argv (argv, error);
  g_strfreev (argv);

  return device;
}

/**
 * usbemu_device_get_attached:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get device attached state.
 *
 * Returns: Device attached state.
 */
gboolean
usbemu_device_get_attached (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), 0);

  return USBEMU_DEVICE_GET_PRIVATE (device)->attached;
}

/**
 * usbemu_device_attach_async:
 * @device: (in): a #UsbemuDevice object.
 * @options: (in) (optional) (array zero-terminated=1): extra information passed
 *           for command line as string-array.
 * @cancellable: (in) (optional): a #GCancellable or %NULL.
 * @callback: (scope async): a #GAsyncReadyCallback.
 * @user_data: (closure): user data for the callback.
 *
 * Asynchronously attach the device with specified options. Use
 * usbemu_device_attach_finish() to retrieve the result.
 */
void
usbemu_device_attach_async (UsbemuDevice         *device,
                            gchar               **options,
                            GCancellable         *cancellable,
                            GAsyncReadyCallback   callback,
                            gpointer              user_data)
{
  GTask *task;
  UsbemuDeviceClass *klass;

  g_return_if_fail (USBEMU_IS_DEVICE (device));

  task = g_task_new (device, cancellable, callback, user_data);
  klass = (UsbemuDeviceClass*) G_OBJECT_GET_CLASS (device);
  if (klass->attach_async != NULL)
    klass->attach_async (device, task, options);
  else
    g_task_return_new_error (task, USBEMU_ERROR, USBEMU_ERROR_NOT_SUPPORTED,
                             "attach operation not supported.");

  g_object_unref (task);
}

/**
 * usbemu_device_attach_finish:
 * @device: (in): a #UsbemuDevice object.
 * @result: (in): the #GAsyncResult.
 * @error: (out) (optional): #GError for error reporting, or %NULL to ignore.
 *
 * Gets the result of a usbemu_device_attach_async() call.
 *
 * Returns: %TRUE if the attachment has been done successfully, %FALSE on error.
 */
gboolean
usbemu_device_attach_finish (UsbemuDevice         *device,
                             GAsyncResult         *result,
                             GError              **error)
{
  g_return_val_if_fail (g_task_is_valid (result, device), FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

/**
 * usbemu_device_detach_async:
 * @device: (in): a #UsbemuDevice object.
 * @options: (in) (optional) (array zero-terminated=1): extra information passed
 *           for command line as string-array.
 * @cancellable: (in) (optional): a #GCancellable or %NULL.
 * @callback: (scope async): a #GAsyncReadyCallback.
 * @user_data: (closure): user data for the callback.
 *
 * Asynchronously detach the device with specified options. Use
 * usbemu_device_detach_finish() to retrieve the result.
 */
void
usbemu_device_detach_async (UsbemuDevice         *device,
                            gchar               **options,
                            GCancellable         *cancellable,
                            GAsyncReadyCallback   callback,
                            gpointer              user_data)
{
  GTask *task;
  UsbemuDeviceClass *klass;

  g_return_if_fail (USBEMU_IS_DEVICE (device));

  task = g_task_new (device, cancellable, callback, user_data);
  klass = (UsbemuDeviceClass*) G_OBJECT_GET_CLASS (device);
  if (klass->detach_async != NULL)
    klass->detach_async (device, task, options);
  else
    g_task_return_new_error (task, USBEMU_ERROR, USBEMU_ERROR_NOT_SUPPORTED,
                             "detach operation not supported.");

  g_object_unref (task);
}

/**
 * usbemu_device_detach_finish:
 * @device: (in): a #UsbemuDevice object.
 * @result: (in): the #GAsyncResult.
 * @error: (out): #GError for error reporting, or %NULL to ignore.
 *
 * Gets the result of a usbemu_device_detach_async() call.
 *
 * Returns: %TRUE if the detachment has been done successfully, %FALSE on error.
 */
gboolean
usbemu_device_detach_finish (UsbemuDevice         *device,
                             GAsyncResult         *result,
                             GError               **error)
{
  g_return_val_if_fail (g_task_is_valid (result, device), FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

void
_usbemu_device_set_attached (UsbemuDevice *device,
                             gboolean      attached,
                             UsbemuSpeeds  speed)
{
  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);

  if (attached == priv->attached)
    return;

  priv->speed = attached ? speed : USBEMU_SPEED_UNKNOWN;
  priv->attached = attached;
  g_object_notify_by_pspec ((GObject*) device, props[PROP_ATTACHED]);
  g_signal_emit (device,
                 signals[attached ? SIGNAL_ATTACHED : SIGNAL_DETACHED], 0);
}

/**
 * usbemu_device_get_specification_num:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get device specification release number in Binary-Coded Decimal (i.e., 2.10
 * is 210H). This field identifies the release of the USB Specification with
 * which the device and its descriptors are compliant.
 *
 * Returns: Device specification number.
 */
guint16
usbemu_device_get_specification_num (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), 0);

  return USBEMU_DEVICE_GET_PRIVATE (device)->bcdUSB;
}

/**
 * usbemu_device_set_specification_num:
 * @device: (in): a #UsbemuDevice object.
 * @spec: device specification number.
 *
 * Set device specification release number in Binary-Coded Decimal (i.e., 2.10
 * is 210H).
 */
void
usbemu_device_set_specification_num (UsbemuDevice *device,
                                     guint16       spec)
{
  g_return_if_fail (USBEMU_IS_DEVICE (device));

  USBEMU_DEVICE_GET_PRIVATE (device)->bcdUSB = spec;
}

/**
 * usbemu_device_get_class:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get device class code (assigned by
 * [USB-IF](http://www.usb.org/developers/defined_class)).
 *
 * If this field is reset to #USBEMU_CLASS_USE_INTERFACE_DESCRIPTOR, each
 * interface within a configuration specifies its own class information and the
 * various interfaces operate independently.
 *
 * If this field is set to a value between #USBEMU_CLASS_AUDIO and
 * #USBEMU_CLASS_APPLICATION_SPECIFIC, the device supports different class
 * specifications on different interfaces and the interfaces may not operate
 * independently. This value identifies the class definition used for the
 * aggregate interfaces.
 *
 * If this field is set to #USBEMU_CLASS_VENDOR_SPECIFIC, the device class is
 * vendor-specific.
 *
 * Returns: Device class code.
 */
UsbemuClasses
usbemu_device_get_class (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), 0);

  return USBEMU_DEVICE_GET_PRIVATE (device)->bDeviceClass;
}

/**
 * usbemu_device_set_class:
 * @device: (in): a #UsbemuDevice object.
 * @klass: device class code.
 *
 * Set device class code.
 */
void
usbemu_device_set_class (UsbemuDevice  *device,
                         UsbemuClasses  klass)
{
  g_return_if_fail (USBEMU_IS_DEVICE (device));

  USBEMU_DEVICE_GET_PRIVATE (device)->bDeviceClass = klass;
}

/**
 * usbemu_device_get_sub_class:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get device sub-class code (assigned by the
 * [USB-IF](http://www.usb.org/developers/defined_class)).
 *
 * These codes are qualified by the value of the class field. If the class field
 * is reset to #USBEMU_CLASS_USE_INTERFACE_DESCRIPTOR, this field must also be
 * reset to #USBEMU_SUB_CLASS_USE_INTERFACE_DESCRIPTOR. If the class field is
 * not set to #USBEMU_CLASS_VENDOR_SPECIFIC, all values are reserved for
 * assignment by the USB-IF.
 *
 * Returns: Device sub-class code.
 */
guint8
usbemu_device_get_sub_class (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), 0);

  return USBEMU_DEVICE_GET_PRIVATE (device)->bDeviceSubClass;
}

/**
 * usbemu_device_set_sub_class:
 * @device: (in): a #UsbemuDevice object.
 * @sub_class: device sub-class code.
 *
 * Set device sub-class code.
 */
void
usbemu_device_set_sub_class (UsbemuDevice *device,
                             guint8        sub_class)
{
  g_return_if_fail (USBEMU_IS_DEVICE (device));

  USBEMU_DEVICE_GET_PRIVATE (device)->bDeviceSubClass = sub_class;
}

/**
 * usbemu_device_get_protocol:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get device protocol code (assigned by the
 * [USB-IF](http://www.usb.org/developers/defined_class)).
 *
 * These codes are qualified by the value of the class and the sub-class fields.
 * If a device supports class-specific protocols on a device basis as opposed to
 * an interface basis, this code identifies the protocols that the device uses
 * as defined by the specification of the device class.
 *
 * If this field is reset to #USBEMU_PROTOCOL_USE_INTERFACE_DESCRIPTOR, the
 * device does not use class-specific protocols on a device basis. However, it
 * may use class-specific protocols on an interface basis.
 *
 * If this field is set to #USBEMU_PROTOCOL_VENDOR_SPECIFIC, the device uses a
 * vendor-specific protocol on a device basis.
 *
 * Returns: Device protocol code.
 */
guint8
usbemu_device_get_protocol (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), 0);

  return USBEMU_DEVICE_GET_PRIVATE (device)->bDeviceProtocol;
}

/**
 * usbemu_device_set_protocol:
 * @device: (in): a #UsbemuDevice object.
 * @protocol: device protocol code
 *
 * Set device protocol code.
 */
void
usbemu_device_set_protocol (UsbemuDevice *device,
                            guint8        protocol)
{
  g_return_if_fail (USBEMU_IS_DEVICE (device));

  USBEMU_DEVICE_GET_PRIVATE (device)->bDeviceProtocol = protocol;
}

/**
 * usbemu_device_get_max_packet_size:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get maximum packet size for endpoint zero (only 8, 16, 32, or 64 are valid).
 *
 * Returns: maximum packet size for endpoint zero.
 */
guint8
usbemu_device_get_max_packet_size (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), 0);

  return USBEMU_DEVICE_GET_PRIVATE (device)->bMaxPacketSize;
}

/**
 * usbemu_device_set_max_packet_size:
 * @device: (in): a #UsbemuDevice object.
 * @max_packet_size: maximum packet size for endpoint zero (only 8, 16, 32, or
 *     64 are valid).
 *
 * Set maximum packet size for endpoint zero.
 */
void
usbemu_device_set_max_packet_size (UsbemuDevice *device,
                                   guint8        max_packet_size)
{
  g_return_if_fail (USBEMU_IS_DEVICE (device));

  USBEMU_DEVICE_GET_PRIVATE (device)->bMaxPacketSize = max_packet_size;
}

/**
 * usbemu_device_get_vendor_id:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get device vendor id (assigned by the
 * [USB-IF](http://www.usb.org/developers/defined_class)).
 *
 * Returns: Device vendor id.
 */
guint16
usbemu_device_get_vendor_id (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), 0);

  return USBEMU_DEVICE_GET_PRIVATE (device)->idVendor;
}

/**
 * usbemu_device_set_vendor_id:
 * @device: (in): a #UsbemuDevice object.
 * @vendor_id: device vendor id.
 *
 * Set device vendor id.
 */
void
usbemu_device_set_vendor_id (UsbemuDevice *device,
                             guint16       vendor_id)
{
  g_return_if_fail (USBEMU_IS_DEVICE (device));

  USBEMU_DEVICE_GET_PRIVATE (device)->idVendor = vendor_id;
}

/**
 * usbemu_device_get_product_id:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get device product id (assigned by the manufacturer).
 *
 * Returns: Device product id.
 */
guint16
usbemu_device_get_product_id (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), 0);

  return USBEMU_DEVICE_GET_PRIVATE (device)->idProduct;
}

/**
 * usbemu_device_set_product_id:
 * @device: (in): a #UsbemuDevice object.
 * @product_id: device product id.
 *
 * Set device product id.
 */
void
usbemu_device_set_product_id (UsbemuDevice *device,
                              guint16       product_id)
{
  g_return_if_fail (USBEMU_IS_DEVICE (device));

  USBEMU_DEVICE_GET_PRIVATE (device)->idProduct = product_id;
}

/**
 * usbemu_device_get_release_number:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get device release number in binary-coded decimal.
 *
 * Returns: Device release number.
 */
guint16
usbemu_device_get_release_number (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), 0);

  return USBEMU_DEVICE_GET_PRIVATE (device)->bcdDevice;
}

/**
 * usbemu_device_set_release_number:
 * @device: (in): a #UsbemuDevice object.
 * @release_number: device release number in binary-coded decimal.
 *
 * Set device release number.
 */
void
usbemu_device_set_release_number (UsbemuDevice *device,
                                  guint16       release_number)
{
  g_return_if_fail (USBEMU_IS_DEVICE (device));

  USBEMU_DEVICE_GET_PRIVATE (device)->bcdDevice = release_number;
}

/**
 * usbemu_device_get_manufacturer_name:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get device manufacturer's name.
 *
 * Returns: (transfer none) Device manufacturer's name. The returned string is
 *          owned by USBEmu and should not be modified or freed.
 */
const gchar*
usbemu_device_get_manufacturer_name (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), NULL);

  return USBEMU_DEVICE_GET_PRIVATE (device)->manufacturer;
}

/**
 * usbemu_device_set_manufacturer_name:
 * @device: (in): a #UsbemuDevice object.
 * @name: (in): a %NULL-terminated name string.
 *
 * Set device manufacturer's name.
 */
void
usbemu_device_set_manufacturer_name (UsbemuDevice *device,
                                     const gchar  *name)
{
  g_return_if_fail (USBEMU_IS_DEVICE (device));

  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);
  if (priv->manufacturer != NULL)
    g_free (priv->manufacturer);
  priv->manufacturer = g_strdup (name);
}

/**
 * usbemu_device_get_product_name:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get device product name.
 *
 * Returns: (transfer none) Device product name. The returned string is
 *          owned by USBEmu and should not be modified or freed.
 */
const gchar*
usbemu_device_get_product_name (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), NULL);

  return USBEMU_DEVICE_GET_PRIVATE (device)->product;
}

/**
 * usbemu_device_set_product_name:
 * @device: (in): a #UsbemuDevice object.
 * @name: (in): a %NULL-terminated name string.
 *
 * Set device product name.
 */
void
usbemu_device_set_product_name (UsbemuDevice *device,
                                const gchar  *name)
{
  g_return_if_fail (USBEMU_IS_DEVICE (device));

  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);
  if (priv->product != NULL)
    g_free (priv->product);
  priv->product = g_strdup (name);
}

/**
 * usbemu_device_get_serial:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get device serial string.
 *
 * Returns: (transfer none) Device device serial string. The returned string is
 *          owned by USBEmu and should not be modified or freed.
 */
const gchar*
usbemu_device_get_serial (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), NULL);

  return USBEMU_DEVICE_GET_PRIVATE (device)->serial;
}

/**
 * usbemu_device_set_serial:
 * @device: (in): a #UsbemuDevice object.
 * @serial: (in): a %NULL-terminated device serial string.
 *
 * Set device serial string.
 */
void
usbemu_device_set_serial (UsbemuDevice *device,
                          const gchar  *serial)
{
  g_return_if_fail (USBEMU_IS_DEVICE (device));

  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);
  if (priv->serial != NULL)
    g_free (priv->serial);
  priv->serial = g_strdup (serial);
}

/**
 * usbemu_device_get_speed:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get current device/bus speed.
 *
 * Returns: A #UsbemuSpeeds.
 */
UsbemuSpeeds
usbemu_device_get_speed (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), USBEMU_SPEED_UNKNOWN);

  return USBEMU_DEVICE_GET_PRIVATE (device)->speed;
}

/**
 * usbemu_device_add_configuration:
 * @device: (in): a #UsbemuDevice object.
 * @configuration: (in): a #UsbemuConfiguration object.
 *
 * Add a configuration to device. The configuration object must not have been
 * added to other device.
 *
 * Returns: %TRUE if succeeded. %FALSE otherwise.
 */
gboolean
usbemu_device_add_configuration (UsbemuDevice        *device,
                                 UsbemuConfiguration *configuration)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), FALSE);
  g_return_val_if_fail (USBEMU_IS_CONFIGURATION (configuration), FALSE);

  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);
  guint bConfigurationValue;

  if (usbemu_configuration_get_configuration_value (configuration) != 0)
    return FALSE;

  g_ptr_array_add (priv->configurations, g_object_ref (configuration));
  bConfigurationValue = priv->configurations->len;
  _usbemu_configuration_set_device (configuration, device, bConfigurationValue);

  return TRUE;
}

/**
 * usbemu_device_get_configuration:
 * @device: (in): a #UsbemuDevice object.
 * @configuration_value: the value used to identify a #UsbemuConfiguration
 *                       object.
 *
 * Get the #UsbemuConfiguration of a device with specified configuration value.
 *
 * Returns: (transfer full): a #UsbemuConfiguration if available, or %NULL
 *          otherwise.
 */
UsbemuConfiguration*
usbemu_device_get_configuration (UsbemuDevice *device,
                                 guint         configuration_value)
{
  UsbemuDevicePrivate *priv;

  g_return_val_if_fail (USBEMU_IS_DEVICE (device), NULL);
  g_return_val_if_fail (configuration_value > 0, NULL);

  priv = USBEMU_DEVICE_GET_PRIVATE (device);
  if (configuration_value > priv->configurations->len)
    return NULL;

  return g_object_ref (g_ptr_array_index (priv->configurations,
                                          configuration_value - 1));
}

/**
 * usbemu_device_get_configurations:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get all available configurations of a device.
 *
 * Returns: (transfer full) (type GPtrArray(UsbemuConfiguration*)): A list of
 *          #UsbemuConfiguration objects added to the device. Free with
 *          g_array_unref().
 */
GPtrArray*
usbemu_device_get_configurations (UsbemuDevice *device)
{
  UsbemuDevicePrivate *priv;
  GPtrArray *ret;
  UsbemuConfiguration *configuration;
  gsize i;

  g_return_val_if_fail (USBEMU_IS_DEVICE (device), NULL);

  priv = USBEMU_DEVICE_GET_PRIVATE (device);
  ret = _create_configurations_array ();
  for (i = 0; i < priv->configurations->len; i++) {
    configuration = g_ptr_array_index (priv->configurations, i);
    g_ptr_array_add (ret, g_object_ref (configuration));
  }

  return ret;
}

/**
 * usbemu_device_get_n_configurations:
 * @device: (in): a #UsbemuDevice object.
 *
 * Get number of available configurations in this device.
 *
 * Returns: number of available configurations.
 */
guint
usbemu_device_get_n_configurations (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), 0);

  return USBEMU_DEVICE_GET_PRIVATE (device)->configurations->len;
}

static void
_free_link_from_queue (GList **head,
                       GList  *list)
{
  g_clear_object (&((UsbemuUrbReal*) list->data)->task);

  usbemu_urb_unref ((UsbemuUrb*) list->data);
  *head = g_list_delete_link (*head, list);
}

static gboolean
_on_urb_loop_timer (UsbemuDevice *device)
{
  UsbemuDevicePrivate *priv;
  GList *current, *next;
  UsbemuUrbReal *rurb;

  priv = USBEMU_DEVICE_GET_PRIVATE (device);
  for (current = priv->urb_queue; current != NULL; current = next) {
    next = current->next;

    rurb = (UsbemuUrbReal*) current->data;
    if (g_task_return_error_if_cancelled (rurb->task)) {
      _free_link_from_queue (&priv->urb_queue, current);
      continue;
    }

    /* FIXME: */
    g_task_return_boolean (rurb->task, TRUE);
    _free_link_from_queue (&priv->urb_queue, current);
  }

  return priv->urb_queue != NULL;
}

static gboolean
_start_urb_loop (UsbemuDevice *device)
{
  if (_on_urb_loop_timer (device)) {
    g_timeout_add_full (G_PRIORITY_DEFAULT, 1,
                        (GSourceFunc) _on_urb_loop_timer,
                        g_object_ref (device),
                        (GDestroyNotify) g_object_unref);
  }

  return FALSE;
}

typedef struct _SubmitUrbData {
  UsbemuUrb *urb;
  UsbemuSubmitUrbReadyCallback callback;
  gpointer user_data;
} SubmitUrbData;

static void
_on_submit_urb_callback (UsbemuDevice  *device,
                         GAsyncResult  *result,
                         SubmitUrbData *data)
{
  if (data->callback != NULL)
    data->callback (device, data->urb, result, data->user_data);

  usbemu_urb_unref (data->urb);
  g_slice_free (SubmitUrbData, data);
}

/**
 * usbemu_device_submit_urb_async:
 * @device: (in): a #UsbemuDevice object.
 * @urb: (in): a #UsbemuUrb object.
 * @cancellable: (in) (optional): a #GCancellable or %NULL.
 * @callback: (scope async): a #UsbemuSubmitUrbReadyCallback.
 * @user_data: (closure): user data for the callback.
 *
 * Submit an @urb to @device.
 */
void
usbemu_device_submit_urb_async (UsbemuDevice                 *device,
                                UsbemuUrb                    *urb,
                                GCancellable                 *cancellable,
                                UsbemuSubmitUrbReadyCallback  callback,
                                gpointer                      user_data)
{
  UsbemuDevicePrivate *priv;
  SubmitUrbData *task_data;
  UsbemuUrbReal *rurb = (UsbemuUrbReal*) urb;

  g_return_if_fail (USBEMU_IS_DEVICE (device));
  g_return_if_fail ((urb != NULL) && (rurb->task == NULL));

  task_data = g_slice_new0 (SubmitUrbData);
  task_data->urb = usbemu_urb_ref (urb);
  task_data->callback = callback;
  task_data->user_data = user_data;
  rurb->task = g_task_new (device, cancellable,
                           (GAsyncReadyCallback) _on_submit_urb_callback,
                           task_data);
  /* manually check cancellable in _on_urb_loop_timer(). */
  g_task_set_check_cancellable (rurb->task, FALSE);

  priv = USBEMU_DEVICE_GET_PRIVATE (device);
  priv->urb_queue = g_list_append (priv->urb_queue, usbemu_urb_ref (urb));
  if (priv->urb_queue->next == NULL) {
    /* urb_queue was empty, kick off _start_urb_loop(). */
    g_idle_add_full (G_PRIORITY_DEFAULT,
                     (GSourceFunc) _start_urb_loop,
                     g_object_ref (device),
                     (GDestroyNotify) g_object_unref);
  }
}

/**
 * usbemu_device_submit_urb_finish:
 * @device: (in): a #UsbemuDevice object.
 * @result: (in): the #GAsyncResult.
 * @error: (out) (optional): #GError for error reporting, or %NULL to ignore.
 *
 * Gets the result of a usbemu_device_submit_urb_async() call.
 *
 * Returns: %TRUE if @urb successfully transferred, %FALSE otherwise.
 */
gboolean
usbemu_device_submit_urb_finish (UsbemuDevice  *device,
                                 GAsyncResult  *result,
                                 GError       **error)
{
  g_return_val_if_fail (g_task_is_valid (result, device), FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}
