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
  int dummy;
} UsbemuDevicePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (UsbemuDevice, usbemu_device, G_TYPE_OBJECT)

#define USBEMU_DEVICE_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), USBEMU_TYPE_DEVICE, UsbemuDevicePrivate))

static void
usbemu_device_class_init (UsbemuDeviceClass *device_class G_GNUC_UNUSED)
{
  /* do nothing */
}

static void
usbemu_device_init (UsbemuDevice *device G_GNUC_UNUSED)
{
  /* do nothing */
}

/**
 * usbemu_device_new:
 *
 * Returns: (transfer full) (type UsbemuDevice): The constructed device object
 *          or %NULL.
 */
UsbemuDevice*
usbemu_device_new ()
{
  return g_object_new (USBEMU_TYPE_DEVICE, NULL);
}
