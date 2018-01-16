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

#include "usbemu/usbemu-device.h"

/**
 * SECTION:usbemu
 * @short_description: USB device emulation library
 * @include usbemu/usbemu.h
 *
 * USBEmu aims to provide #GObject-based API for USB device emulation initially
 * based on function provided by Linux kernel vhci-hcd module.
 */

/**
 * SECTION:usbemu-device
 * @short_description: USB device emulation class
 * @include usbemu/usbemu.h
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
 *
 * Class structure for UsbemuDevice.
 */

typedef struct  _UsbemuDevicePrivate {
  gboolean attached;
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
static void gobject_class_finalize (GObject *object);
/* virtual methods for UsbemuDeviceClass */
static void usbemu_device_class_init (UsbemuDeviceClass *device_class);

static void
gobject_class_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  UsbemuDevice *device = USBEMU_DEVICE (object);
  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);

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
gobject_class_finalize (GObject *object G_GNUC_UNUSED)
{
}

static void
usbemu_device_class_init (UsbemuDeviceClass *device_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (device_class);

  /* virtual methods */

  object_class->set_property = gobject_class_set_property;
  object_class->get_property = gobject_class_get_property;
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

static void
usbemu_device_init (UsbemuDevice *device)
{
  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);

  priv->attached = FALSE;
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

void
_usbemu_device_set_attached (UsbemuDevice *device,
                             gboolean      attached)
{
  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);

  if (attached == priv->attached)
    return;

  priv->attached = attached;
  g_object_notify_by_pspec ((GObject*) device, props[PROP_ATTACHED]);
  g_signal_emit (device,
                 signals[attached ? SIGNAL_ATTACHED : SIGNAL_DETACHED], 0);
}
