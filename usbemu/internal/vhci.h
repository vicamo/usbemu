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

#if !defined (LIBUSBEMU_COMPILATION)
#error "libusbemu internal use only."
#endif

#include <linux/usb/ch9.h>

#include <glib.h>

gboolean _vhci_attach_device (guint                  *port,
                              gint                    sockfd,
                              guint32                 devid,
                              enum usb_device_speed   speed,
                              GError                **error);
gboolean _vhci_detach_device (guint                   port,
                              GError                **error);
