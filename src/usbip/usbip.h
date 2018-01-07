/* usbemu - USB Emulation Library
 * Copyright (C) 2016 You-Sheng Yang
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details:
 */

#pragma once

#include "usbemu/dbus/usbemu-dbus-manager.h"

G_BEGIN_DECLS

void usbip_dbus_init (const gchar         *address,
                      GDBusObjectManager **object_manager,
                      UsbemuDBusManager  **manager);

G_END_DECLS
