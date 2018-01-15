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

struct _UsbemuInterface;

/**
 * UsbemuConfigurationAttributes:
 * @USBEMU_CONFIGURATION_ATTR_RESERVED_0: Reserved.
 * @USBEMU_CONFIGURATION_ATTR_RESERVED_1: Reserved.
 * @USBEMU_CONFIGURATION_ATTR_RESERVED_2: Reserved.
 * @USBEMU_CONFIGURATION_ATTR_RESERVED_3: Reserved.
 * @USBEMU_CONFIGURATION_ATTR_RESERVED_4: Reserved.
 * @USBEMU_CONFIGURATION_ATTR_REMOTE_WAKEUP: Set if a device configuration
 *     supports remote wakeup.
 * @USBEMU_CONFIGURATION_ATTR_SELF_POWER: A device configuration that uses power
 *     from the bus and a local source reports a non-zero value in
 *     #UsbemuConfiguration:max-power to indicate the amount of bus power
 *     required.
 * @USBEMU_CONFIGURATION_ATTR_RESERVED_7: Reserved and must be set to for
 *     historical reasons.
 *
 * Configuration characteristics.
 */
typedef enum /*< flags,prefix=USBEMU >*/
{
  USBEMU_CONFIGURATION_ATTR_RESERVED_0 = (0x1 << 0), /*< nick=RESERVED_0 >*/
  USBEMU_CONFIGURATION_ATTR_RESERVED_1 = (0x1 << 1), /*< nick=RESERVED_1 >*/
  USBEMU_CONFIGURATION_ATTR_RESERVED_2 = (0x1 << 2), /*< nick=RESERVED_2 >*/
  USBEMU_CONFIGURATION_ATTR_RESERVED_3 = (0x1 << 3), /*< nick=RESERVED_3 >*/
  USBEMU_CONFIGURATION_ATTR_RESERVED_4 = (0x1 << 4), /*< nick=RESERVED_4 >*/
  USBEMU_CONFIGURATION_ATTR_REMOTE_WAKEUP = (0x1 << 5), /*< nick=REMOTE_WAKEUP >*/
  USBEMU_CONFIGURATION_ATTR_SELF_POWER = (0x1 << 6), /*< nick=SELF_POWER >*/
  USBEMU_CONFIGURATION_ATTR_RESERVED_7 = (0x1 << 7), /*< nick=RESERVED_7 >*/
} UsbemuConfigurationAttributes;

UsbemuConfiguration* usbemu_configuration_new      ();
UsbemuConfiguration* usbemu_configuration_new_full (const gchar *name,
                                                    guint        attributes,
                                                    guint        max_power);

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

UsbemuDevice* usbemu_configuration_get_device (UsbemuConfiguration *configuration);

gint    usbemu_configuration_add_alternate_interfaces   (UsbemuConfiguration      *configuration,
                                                         struct _UsbemuInterface **interfaces);
GSList* usbemu_configuration_get_alternate_interfaces   (UsbemuConfiguration      *configuration,
                                                         guint                     interface_number);
guint   usbemu_configuration_get_n_alternate_interfaces (UsbemuConfiguration      *configuration);

G_END_DECLS
