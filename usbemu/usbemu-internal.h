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

#if defined (__USBEMU_USBEMU_H_INSIDE__)
#error "usbemu-internal.h accidentally included in public headers."
#endif

#include "usbemu/usbemu-configuration.h"
#include "usbemu/usbemu-device.h"

/**
 * SECTION:usbemu-internal
 * @short_description: internal functions for libusbemu
 * @stability: internal
 *
 * Internal used functions. Mainly protected member functions.
 */

G_BEGIN_DECLS

void _usbemu_device_set_attached (UsbemuDevice *device,
                                  gboolean      attached);

void _usbemu_configuration_set_device (UsbemuConfiguration *configuration,
                                       UsbemuDevice        *device,
                                       guint                configuration_value);

G_END_DECLS
