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
#include <gio/gio.h>

G_BEGIN_DECLS

#define USBEMU_TYPE_DEVICE  (usbemu_device_get_type ())

G_DECLARE_DERIVABLE_TYPE (UsbemuDevice, usbemu_device, USBEMU, DEVICE,
                          GObject)

#define USBEMU_DEVICE_PROP_UDI "udi"

struct _UsbemuDeviceClass {
  GObjectClass parent_class;

  /*< private >*/

  /* Reserved slots for furture extension. */
  gpointer padding[12];
};

UsbemuDevice* usbemu_device_new (const gchar *udi);

const gchar* usbemu_device_get_udi (UsbemuDevice *device);

void     usbemu_device_bind          (UsbemuDevice        *device,
                                      GCancellable        *cancellable,
                                      GAsyncReadyCallback  callback,
                                      gpointer             user_data);
gboolean usbemu_device_bind_finish   (GAsyncResult        *result,
                                      GError             **error);
void     usbemu_device_unbind        (UsbemuDevice        *device,
                                      GCancellable        *cancellable,
                                      GAsyncReadyCallback  callback,
                                      gpointer             user_data);
gboolean usbemu_device_unbind_finish (GAsyncResult        *result,
                                      GError             **error);
gboolean usbemu_device_get_bound     (UsbemuDevice        *device);

G_END_DECLS
