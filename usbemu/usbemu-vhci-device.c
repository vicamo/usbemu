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

#include "usbemu/usbemu-device.h"
#include "usbemu/usbemu-vhci-device.h"

/**
 * SECTION:usbemu-vhci-device
 * @short_description: USB VHCI device emulation class
 * @include: usbemu/usbemu.h
 *
 * #UsbemuVhciDevice provides USB emulation for local system.
 */

#define LOG_COMPONENT "VHCI: "

/**
 * UsbemuVhciDevice:
 *
 * An object representing emulated USB device for Linux USB VHCI.
 */

/**
 * UsbemuVhciDeviceClass:
 * @parent_class: the parent class.
 *
 * Class structure for UsbemuVhciDevice.
 */

typedef struct  _UsbemuVhciDevicePrivate {
  guint32 devid;
  guint port;
} UsbemuVhciDevicePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (UsbemuVhciDevice, usbemu_vhci_device,
                            USBEMU_TYPE_DEVICE)

#define USBEMU_VHCI_DEVICE_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), USBEMU_TYPE_VHCI_DEVICE, \
                                UsbemuVhciDevicePrivate))

enum
{
  PROP_0,
  PROP_DEVID,
  PROP_PORT,
  N_PROPERTIES
};

static GParamSpec *props[N_PROPERTIES] = { NULL, };

/* virtual methods for GObjectClass */
static void object_class_set_property (GObject *object, guint prop_id,
                                       const GValue *value, GParamSpec *pspec);
static void object_class_get_property (GObject *object, guint prop_id,
                                       GValue *value, GParamSpec *pspec);
/* init functions */
static void usbemu_vhci_device_class_init (UsbemuVhciDeviceClass *klass);
static void usbemu_vhci_device_init (UsbemuVhciDevice *vhci_device);

static void
object_class_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  UsbemuVhciDevice *vhci_device = USBEMU_VHCI_DEVICE (object);
  UsbemuVhciDevicePrivate *priv = USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device);

  switch (prop_id) {
    case PROP_DEVID:
      priv->devid = g_value_get_uint (value);
      break;
    case PROP_PORT:
      priv->port = g_value_get_uint (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
object_class_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  UsbemuVhciDevice *vhci_device = USBEMU_VHCI_DEVICE (object);

  switch (prop_id) {
    case PROP_DEVID:
      g_value_set_uint (value, usbemu_vhci_device_get_devid (vhci_device));
      break;
    case PROP_PORT:
      g_value_set_uint (value, usbemu_vhci_device_get_port (vhci_device));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
usbemu_vhci_device_class_init (UsbemuVhciDeviceClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  /* virtual methods */

  object_class->set_property = object_class_set_property;
  object_class->get_property = object_class_get_property;

  /* properties */

  /**
   * UsbemuVhciDevice:devid:
   */
  props[PROP_DEVID] =
    g_param_spec_uint (USBEMU_VHCI_DEVICE_PROP_DEVID,
                       "Device ID", "Device ID",
                       0, G_MAXUINT, 0,
                       G_PARAM_READWRITE | \
                         G_PARAM_CONSTRUCT);

  /**
   * UsbemuVhciDevice:port:
   *
   * Port number this VHCI device is attached to.
   */
  props[PROP_PORT] =
    g_param_spec_uint (USBEMU_VHCI_DEVICE_PROP_PORT,
                       "Port", "Port",
                       0, G_MAXUINT, USBEMU_VHCI_DEVICE_PORT_UNSPECIFIED,
                       G_PARAM_READABLE);

  g_object_class_install_properties (object_class, N_PROPERTIES, props);
}

static void
usbemu_vhci_device_init (UsbemuVhciDevice *vhci_device)
{
  UsbemuVhciDevicePrivate *priv = USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device);

  priv->devid = 0;
  priv->port = USBEMU_VHCI_DEVICE_PORT_UNSPECIFIED;
}

/**
 * usbemu_vhci_device_new:
 *
 * Create a new #UsbemuVhciDevice instance.
 *
 * Returns: (transfer full) (type UsbemuVhciDevice): The constructed device
 *          object or %NULL if failed.
 */
UsbemuVhciDevice*
usbemu_vhci_device_new ()
{
  return g_object_new (USBEMU_TYPE_VHCI_DEVICE, NULL);
}

/**
 * usbemu_vhci_device_get_devid:
 * @vhci_device: (in): A #UsbemuVhciDevice object.
 *
 * Get VHCI device id.
 *
 * Returns: VHCI Device devid.
 */
guint32
usbemu_vhci_device_get_devid (UsbemuVhciDevice *vhci_device)
{
  g_return_val_if_fail (USBEMU_IS_VHCI_DEVICE (vhci_device), 0);

  return USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device)->devid;
}

/**
 * usbemu_vhci_device_set_devid:
 * @vhci_device: (in): A #UsbemuVhciDevice object.
 * @devid: unique device identifier.
 *
 * Set VHCI device id. devid is unique device identifier in a remote host in
 * usbip protocol. In USBEmu it's irrelevant because we have no physical device
 * to identify at all.
 */
void
usbemu_vhci_device_set_devid (UsbemuVhciDevice *vhci_device,
                              guint32           devid)
{
  g_return_if_fail (USBEMU_IS_VHCI_DEVICE (vhci_device));

  USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device)->devid = devid;
}

/**
 * usbemu_vhci_device_get_port:
 * @vhci_device: (in): a #UsbemuVhciDevice object.
 *
 * Get VHCI Device port.
 *
 * Returns: VHCI Device port.
 */
guint
usbemu_vhci_device_get_port (UsbemuVhciDevice *vhci_device)
{
  g_return_val_if_fail (USBEMU_IS_VHCI_DEVICE (vhci_device),
                        USBEMU_VHCI_DEVICE_PORT_UNSPECIFIED);

  return USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device)->port;
}

static void
_usbemu_vhci_device_set_port (UsbemuVhciDevice *vhci_device,
                              guint             port)
{
  USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device)->port = port;
  g_object_notify_by_pspec (G_OBJECT (vhci_device), props[PROP_PORT]);
}
