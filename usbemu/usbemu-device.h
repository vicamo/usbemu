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

#include <usbemu/usbemu-configuration.h>
#include <usbemu/usbemu-urb.h>
#include <usbemu/usbemu-usb.h>

G_BEGIN_DECLS

/**
 * USBEMU_TYPE_DEVICE:
 *
 * Convenient macro for usbemu_device_get_type().
 */
#define USBEMU_TYPE_DEVICE  (usbemu_device_get_type ())

G_DECLARE_DERIVABLE_TYPE (UsbemuDevice, usbemu_device, USBEMU, DEVICE,
                          GObject)

/**
 * USBEMU_DEVICE_PROP_ATTACHED:
 *
 * "attached" property name.
 */
#define USBEMU_DEVICE_PROP_ATTACHED "attached"

/**
 * USBEMU_DEVICE_SIGNAL_ATTACHED:
 *
 * "attached" signal name.
 */
#define USBEMU_DEVICE_SIGNAL_ATTACHED "attached"
/**
 * USBEMU_DEVICE_SIGNAL_DETACHED:
 *
 * "detached" signal name.
 */
#define USBEMU_DEVICE_SIGNAL_DETACHED "detached"

struct _UsbemuDeviceClass {
  GObjectClass parent_class;

  /* signal callbacks */

  void (*attached) (UsbemuDevice *device);
  void (*detached) (UsbemuDevice *device);

  /* virtual methods */

  void (*attach_async) (UsbemuDevice  *device,
                        GTask         *task,
                        gchar        **options);
  void (*detach_async) (UsbemuDevice  *device,
                        GTask         *task,
                        gchar        **options);

  /*< private >*/

  /* Reserved slots for furture extension. */
  gpointer padding[12];
};

/**
 * UsbemuSubmitUrbReadyCallback:
 * @device: (in): a #UsbemuDevice object.
 * @urb: (in): a #UsbemuUrb object.
 * @result: (in): the #GAsyncResult.
 * @user_data: (in): user data passed to the callback.
 *
 * Type definition for a function that will be called back when a previous
 * usbemu_device_submit_urb_async() call has been completed.
 */
typedef void (*UsbemuSubmitUrbReadyCallback) (UsbemuDevice *device,
                                              UsbemuUrb    *urb,
                                              GAsyncResult *result,
                                              gpointer      user_data);

UsbemuDevice* usbemu_device_new             ();
UsbemuDevice* usbemu_device_new_from_argv   (gchar       **argv,
                                             GError      **error);
UsbemuDevice* usbemu_device_new_from_string (const gchar  *str,
                                             GError      **error);

gboolean usbemu_device_get_attached  (UsbemuDevice         *device);
void     usbemu_device_attach_async  (UsbemuDevice         *device,
                                      gchar               **options,
                                      GCancellable         *cancellable,
                                      GAsyncReadyCallback   callback,
                                      gpointer              user_data);
gboolean usbemu_device_attach_finish (UsbemuDevice         *device,
                                      GAsyncResult         *result,
                                      GError              **error);
void     usbemu_device_detach_async  (UsbemuDevice         *device,
                                      gchar               **options,
                                      GCancellable         *cancellable,
                                      GAsyncReadyCallback   callback,
                                      gpointer              user_data);
gboolean usbemu_device_detach_finish (UsbemuDevice         *device,
                                      GAsyncResult         *result,
                                      GError               **error);

guint16       usbemu_device_get_specification_num (UsbemuDevice  *device);
void          usbemu_device_set_specification_num (UsbemuDevice  *device,
                                                   guint16        spec);
UsbemuClasses usbemu_device_get_class             (UsbemuDevice  *device);
void          usbemu_device_set_class             (UsbemuDevice  *device,
                                                   UsbemuClasses  klass);
guint8        usbemu_device_get_sub_class         (UsbemuDevice  *device);
void          usbemu_device_set_sub_class         (UsbemuDevice  *device,
                                                   guint8         sub_class);
guint8        usbemu_device_get_protocol          (UsbemuDevice  *device);
void          usbemu_device_set_protocol          (UsbemuDevice  *device,
                                                   guint8         protocol);
guint8        usbemu_device_get_max_packet_size   (UsbemuDevice  *device);
void          usbemu_device_set_max_packet_size   (UsbemuDevice  *device,
                                                   guint8         max_packet_size);
guint16       usbemu_device_get_vendor_id         (UsbemuDevice  *device);
void          usbemu_device_set_vendor_id         (UsbemuDevice  *device,
                                                   guint16        vendor_id);
guint16       usbemu_device_get_product_id        (UsbemuDevice  *device);
void          usbemu_device_set_product_id        (UsbemuDevice  *device,
                                                   guint16        product_id);
guint16       usbemu_device_get_release_number    (UsbemuDevice  *device);
void          usbemu_device_set_release_number    (UsbemuDevice  *device,
                                                   guint16        release_number);
const gchar*  usbemu_device_get_manufacturer_name (UsbemuDevice  *device);
void          usbemu_device_set_manufacturer_name (UsbemuDevice  *device,
                                                   const gchar   *name);
const gchar*  usbemu_device_get_product_name      (UsbemuDevice  *device);
void          usbemu_device_set_product_name      (UsbemuDevice  *device,
                                                   const gchar   *name);
const gchar*  usbemu_device_get_serial            (UsbemuDevice  *device);
void          usbemu_device_set_serial            (UsbemuDevice  *device,
                                                   const gchar   *serial);
UsbemuSpeeds  usbemu_device_get_speed             (UsbemuDevice  *device);

gboolean             usbemu_device_add_configuration    (UsbemuDevice        *device,
                                                         UsbemuConfiguration *configuration);
UsbemuConfiguration* usbemu_device_get_configuration    (UsbemuDevice        *device,
                                                         guint                configuration_value);
GPtrArray*           usbemu_device_get_configurations   (UsbemuDevice        *device);
guint                usbemu_device_get_n_configurations (UsbemuDevice        *device);

void     usbemu_device_submit_urb_async  (UsbemuDevice                  *device,
                                          UsbemuUrb                     *urb,
                                          GCancellable                  *cancellable,
                                          UsbemuSubmitUrbReadyCallback   callback,
                                          gpointer                       user_data);
gboolean usbemu_device_submit_urb_finish (UsbemuDevice                  *device,
                                          GAsyncResult                  *result,
                                          GError                       **error);

G_END_DECLS
