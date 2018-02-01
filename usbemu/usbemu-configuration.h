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

#include <usbemu/usbemu-interface.h>
#include <usbemu/usbemu-usb.h>

G_BEGIN_DECLS

/**
 * USBEMU_TYPE_CONFIGURATION:
 *
 * Convenient macro for usbemu_configuration_get_type().
 */
#define USBEMU_TYPE_CONFIGURATION  (usbemu_configuration_get_type ())

G_DECLARE_FINAL_TYPE (UsbemuConfiguration, usbemu_configuration,
                      USBEMU, CONFIGURATION, GObject)

/**
 * USBEMU_CONFIGURATION_PROP_NAME:
 *
 * "name" property name.
 */
#define USBEMU_CONFIGURATION_PROP_NAME "name"
/**
 * USBEMU_CONFIGURATION_PROP_ATTRIBUTES:
 *
 * "attributes" property name.
 */
#define USBEMU_CONFIGURATION_PROP_ATTRIBUTES "attributes"
/**
 * USBEMU_CONFIGURATION_PROP_MAX_POWER:
 *
 * "max-power" property name.
 */
#define USBEMU_CONFIGURATION_PROP_MAX_POWER "max-power"

#ifndef __GTK_DOC_IGNORE__
typedef struct _UsbemuDevice UsbemuDevice;
#endif

UsbemuConfiguration* usbemu_configuration_new             ();
UsbemuConfiguration* usbemu_configuration_new_full        (const gchar *name,
                                                           guint        attributes,
                                                           guint        max_power);
UsbemuConfiguration* usbemu_configuration_new_from_argv   (gchar       **argv,
                                                           GError      **error);
UsbemuConfiguration* usbemu_configuration_new_from_string (const gchar  *str,
                                                           GError      **error);

/* Fields on standard configuration descriptor. */
guint        usbemu_configuration_get_configuration_value (UsbemuConfiguration *configuration);
const gchar* usbemu_configuration_get_name                (UsbemuConfiguration *configuration);
void         usbemu_configuration_set_name                (UsbemuConfiguration *configuration,
                                                           const gchar         *name);
guint        usbemu_configuration_get_attributes          (UsbemuConfiguration *configuration);
void         usbemu_configuration_set_attributes          (UsbemuConfiguration *configuration,
                                                           guint                attributes);
guint        usbemu_configuration_get_max_power           (UsbemuConfiguration *configuration);
void         usbemu_configuration_set_max_power           (UsbemuConfiguration *configuration,
                                                           guint                max_power);

UsbemuDevice*    usbemu_configuration_get_device (UsbemuConfiguration *configuration);

guint            usbemu_configuration_get_n_interface_groups     (UsbemuConfiguration *configuration);
gboolean         usbemu_configuration_add_alternate_interface    (UsbemuConfiguration *configuration,
                                                                  guint                interface_number,
                                                                  UsbemuInterface     *interface);
UsbemuInterface* usbemu_configuration_get_alternate_interface    (UsbemuConfiguration *configuration,
                                                                  guint                interface_number,
                                                                  guint                alternate_setting);
GPtrArray*       usbemu_configuration_get_alternate_interfaces   (UsbemuConfiguration *configuration,
                                                                  guint                interface_number);
guint            usbemu_configuration_get_n_alternate_interfaces (UsbemuConfiguration *configuration,
                                                                  guint                interface_number);

G_END_DECLS
