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

#pragma once

#if !defined (__USBEMU_USBEMU_H_INSIDE__) && !defined (LIBUSBEMU_COMPILATION)
#error "Only <usbemu/usbemu.h> can be included directly."
#endif

#include <glib-object.h>

#include <usbemu/usbemu-device.h>

G_BEGIN_DECLS

/**
 * USBEMU_TYPE_VHCI_DEVICE:
 *
 * Convenient macro for usbemu_vhci_device_get_type().
 */
#define USBEMU_TYPE_VHCI_DEVICE  (usbemu_vhci_device_get_type ())

G_DECLARE_DERIVABLE_TYPE (UsbemuVhciDevice, usbemu_vhci_device,
                          USBEMU, VHCI_DEVICE, UsbemuDevice)

/**
 * USBEMU_VHCI_DEVICE_PROP_DEVID:
 *
 * "devid" property name.
 */
#define USBEMU_VHCI_DEVICE_PROP_DEVID "devid"
/**
 * USBEMU_VHCI_DEVICE_PROP_PORT:
 *
 * "port" property name.
 */
#define USBEMU_VHCI_DEVICE_PROP_PORT "port"

/**
 * USBEMU_VHCI_DEVICE_PORT_UNSPECIFIED:
 *
 * Initial value for #UsbemuVhciDevice:port of an unattached device.
 */
#define USBEMU_VHCI_DEVICE_PORT_UNSPECIFIED  G_MAXUINT

struct _UsbemuVhciDeviceClass {
  UsbemuDeviceClass parent_class;

  /*< private >*/

  /* Reserved slots for furture extension. */
  gpointer padding[12];
};

UsbemuVhciDevice* usbemu_vhci_device_new ();

guint32 usbemu_vhci_device_get_devid (UsbemuVhciDevice *vhci_device);
void    usbemu_vhci_device_set_devid (UsbemuVhciDevice *vhci_device,
                                      guint32           devid);
guint   usbemu_vhci_device_get_port  (UsbemuVhciDevice *vhci_device);

gboolean usbemu_vhci_device_detach_port (guint    port,
                                         GError **error);

G_END_DECLS
