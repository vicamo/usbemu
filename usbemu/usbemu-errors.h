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

#pragma once

#if !defined (__USBEMU_USBEMU_H_INSIDE__) && !defined (LIBUSBEMU_COMPILATION)
#error "Only <usbemu/usbemu.h> can be included directly."
#endif

#include <glib-object.h>

#define USBEMU_DBUS_PATH "/org/usbemu"

/* Prefix for object paths */
#define USBEMU_DBUS_MANAGER_PATH USBEMU_DBUS_PATH "/UsbemuManager"
#define USBEMU_DBUS_DEVICE_PATH  USBEMU_DBUS_MANAGER_PATH "/Devices"

#define USBEMU_DBUS_PREFIX "org.usbemu.UsbemuManager"

/* Prefix for interfaces */
#define USBEMU_DBUS_DEVICE_PREFIX USBEMU_DBUS_PREFIX ".Device"

/* Prefix for DBus errors */
#define USBEMU_DBUS_ERROR_PREFIX USBEMU_DBUS_PREFIX ".Error"

/**
 * UsbemuManagerError:
 * @USBEMU_MANAGER_ERROR_FAILED: unknown or unclassified failure.
 * @USBEMU_MANAGER_ERROR_DEVICE_UNAVAILABLE: device unavailable.
 *
 * Errors related to the UsbemuManager interface of UsbemuManager DBus service.
 */
typedef enum { /*< underscore_name=usbemu_manager_error >*/
  USBEMU_MANAGER_ERROR_FAILED = 0, /*< nick=Failed >*/
  USBEMU_MANAGER_ERROR_DEVICE_UNAVAILABLE, /*< nick=DeviceUnavailable >*/
} UsbemuManagerError;

GQuark usbemu_manager_error_quark (void);
#define USBEMU_MANAGER_ERROR (usbemu_manager_error_quark ())
#define USBEMU_MANAGER_ERROR_DBUS_PREFIX USBEMU_DBUS_ERROR_PREFIX ".Manager"
