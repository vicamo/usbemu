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

#include "config.h"

#include "usbemu/usbemu-device.h"

/**
 * UsbemuDevice:
 *
 * An object representing emulated USB device.
 */

/**
 * UsbemuDeviceClass:
 * @parent_class: The parent class.
 *
 * Class structure for UsbemuDevice.
 */

typedef struct  _UsbemuDevicePrivate {
  gchar *udi;
} UsbemuDevicePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (UsbemuDevice, usbemu_device, G_TYPE_OBJECT)

#define USBEMU_DEVICE_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), USBEMU_TYPE_DEVICE, UsbemuDevicePrivate))

enum
{
  PROP_0,
  PROP_UDI,
  N_PROPERTIES
};

static GParamSpec *props[N_PROPERTIES] = { NULL, };

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
  UsbemuDevice *device = USBEMU_DEVICE (object);
  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);

  switch (prop_id) {
    case PROP_UDI:
      g_assert (priv->udi == NULL);
      priv->udi = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
  UsbemuDevice *device = USBEMU_DEVICE (object);

  switch (prop_id) {
    case PROP_UDI:
      g_value_set_string (value, usbemu_device_get_udi (device));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
finalize (GObject *object)
{
  UsbemuDevice *device = USBEMU_DEVICE (object);
  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);

  g_free (priv->udi);
  priv->udi = NULL;
}

static void
usbemu_device_class_init (UsbemuDeviceClass *device_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (device_class);

  /* virtual methods */

  object_class->set_property = set_property;
  object_class->get_property = get_property;
  object_class->finalize = finalize;

  /* properties */

  /**
   * UsbemuDevice:udi:
   *
   * Operating-system specific transient device hardware identifier.
   */
  props[PROP_UDI] =
    g_param_spec_string (USBEMU_DEVICE_PROP_UDI, "Udi", "Udi",
                         NULL,
                         G_PARAM_READWRITE | \
                           G_PARAM_CONSTRUCT_ONLY | \
                           G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPERTIES, props);
}

static void
usbemu_device_init (UsbemuDevice *device G_GNUC_UNUSED)
{
  /* do nothing */
}

/**
 * usbemu_device_new:
 * @udi: A non-%NULL string.
 *
 * Returns: (transfer full) (type UsbemuDevice): The constructed device object
 *          or %NULL.
 */
UsbemuDevice*
usbemu_device_new (const gchar *udi)
{
  g_return_val_if_fail (udi != NULL, NULL);

  return g_object_new (USBEMU_TYPE_DEVICE, USBEMU_DEVICE_PROP_UDI, udi, NULL);
}

/**
 * usbemu_device_get_udi:
 * @device: A UsbemuDevice object.
 *
 * Returns: Device UDI string. The returned string is owned by USBEmu and
 *          should not be modified or freed.
 */
const gchar*
usbemu_device_get_udi (UsbemuDevice *device)
{
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), NULL);

  return USBEMU_DEVICE_GET_PRIVATE (device)->udi;
}
