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
#error "usbemu/internal/utils.h accidentally included in public headers."
#endif

#include <glib.h>
#include <glib-object.h>

#include "usbemu/usbemu-configuration.h"
#include "usbemu/usbemu-device.h"
#include "usbemu/usbemu-interface.h"

G_BEGIN_DECLS

UsbemuDevice* _usbemu_device_new_from_argv_inner (gchar    ***argv,
                                                  GError    **error,
                                                  gboolean    allow_remaining);
void _usbemu_device_set_attached (UsbemuDevice *device,
                                  gboolean      attached,
                                  UsbemuSpeeds  speed);

UsbemuConfiguration* _usbemu_configuration_new_from_argv_inner (gchar    ***argv,
                                                                GError    **error,
                                                                gboolean    allow_remaining);
void _usbemu_configuration_set_device (UsbemuConfiguration *configuration,
                                       UsbemuDevice        *device,
                                       guint                configuration_value);

UsbemuInterface* _usbemu_interface_new_from_argv_inner (gchar    ***argv,
                                                        GError    **error,
                                                        gboolean    allow_remaining);
void _usbemu_interface_set_configuration (UsbemuInterface     *interface,
                                          UsbemuConfiguration *configuration,
                                          guint                interface_number,
                                          guint                alternate_setting);

void _usbemu_free_dereferenced (gchar **data);
void _usbemu_object_unref_dereferenced (GObject **object);
GObject* _usbemu_object_new_from_argv (gchar       ***argv,
                                       GType         *base_type,
                                       const gchar   *type_key,
                                       GError       **error);

G_END_DECLS
