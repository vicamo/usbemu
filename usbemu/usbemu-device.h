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

struct _UsbemuConfiguration;

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
 * UsbemuClasses:
 * @USBEMU_CLASS_USE_INTERFACE_DESCRIPTOR:
 *     Use class information in the Interface Descriptors. See
 *     [BaseClass00h](http://www.usb.org/developers/defined_class/#BaseClass00h).
 * @USBEMU_CLASS_AUDIO:
 *     Audio. See
 *     [BaseClass01h](http://www.usb.org/developers/defined_class/#BaseClass01h).
 * @USBEMU_CLASS_COMMUNICATIONS_AND_CDC_CONTROL:
 *     Communications and CDC Control. See
 *     [BaseClass02h](http://www.usb.org/developers/defined_class/#BaseClass02h).
 * @USBEMU_CLASS_HID:
 *     HID (Human Interface Device). See
 *     [BaseClass03h](http://www.usb.org/developers/defined_class/#BaseClass03h).
 * @USBEMU_CLASS_PHYSICAL:
 *     Physical. See
 *     [BaseClass05h](http://www.usb.org/developers/defined_class/#BaseClass05h).
 * @USBEMU_CLASS_IMAGE:
 *     Image. See
 *     [BaseClass06h](http://www.usb.org/developers/defined_class/#BaseClass06h)
 * @USBEMU_CLASS_PRINTER:
 *     Printer. See
 *     [BaseClass07h](http://www.usb.org/developers/defined_class/#BaseClass07h).
 * @USBEMU_CLASS_MASS_STORAGE:
 *     Mass Storage. See
 *     [BaseClass08h](http://www.usb.org/developers/defined_class/#BaseClass08h).
 * @USBEMU_CLASS_HUB:
 *     Hub. See
 *     [BaseClass09h](http://www.usb.org/developers/defined_class/#BaseClass09h).
 * @USBEMU_CLASS_CDC_DATA:
 *     CDC-Data. See
 *     [BaseClass0Ah](http://www.usb.org/developers/defined_class/#BaseClass0Ah).
 * @USBEMU_CLASS_SMART_CARD:
 *     Smart Card. See
 *     [BaseClass0Bh](http://www.usb.org/developers/defined_class/#BaseClass0Bh).
 * @USBEMU_CLASS_CONTENT_SECURITY:
 *     Content Security. See
 *     [BaseClass0Dh](http://www.usb.org/developers/defined_class/#BaseClass0Dh).
 * @USBEMU_CLASS_VIDEO:
 *     Video. See
 *     [BaseClass0Eh](http://www.usb.org/developers/defined_class/#BaseClass0Eh).
 * @USBEMU_CLASS_PERSONAL_HEALTHCARE:
 *     Personal Healthcare. See
 *     [BaseClass0Fh](http://www.usb.org/developers/defined_class/#BaseClass0Fh).
 * @USBEMU_CLASS_AUDIO_VIDEO:
 *     Audio/Video Devices. See
 *     [BaseClass10h](http://www.usb.org/developers/defined_class/#BaseClass10h).
 * @USBEMU_CLASS_BILLBOARD:
 *     Billboard Devices Class. See
 *     [BaseClass11h](http://www.usb.org/developers/defined_class/#BaseClass11h).
 * @USBEMU_CLASS_USB_TYPE_C_BRIDGE:
 *     USB Type-C Bridge Class. See
 *     [BaseClass12h](http://www.usb.org/developers/defined_class/#BaseClass12h).
 * @USBEMU_CLASS_DIAGNOSTIC:
 *     Diagnostic Device. See
 *     [BaseClassDCh](http://www.usb.org/developers/defined_class/#BaseClassDCh).
 * @USBEMU_CLASS_WIRELESS_CONTROLLER:
 *     Wireless Controller. See
 *     [BaseClassE0h](http://www.usb.org/developers/defined_class/#BaseClassE0h).
 * @USBEMU_CLASS_MISCELLANEOUS:
 *     Miscellaneous. See
 *     [BaseClassEFh](http://www.usb.org/developers/defined_class/#BaseClassEFh).
 * @USBEMU_CLASS_APPLICATION_SPECIFIC:
 *     Application Specific. See
 *     [BaseClassFEh](http://www.usb.org/developers/defined_class/#BaseClassFEh).
 * @USBEMU_CLASS_VENDOR_SPECIFIC:
 *     Vendor Specific. See
 *     [BaseClass00h](http://www.usb.org/developers/defined_class/#BaseClassFFh).
 *
 * USB class code (assigned by [USB-IF](http://www.usb.org/developers/defined_class)).
 */
typedef enum /*< enum,prefix=USBEMU >*/
{
  USBEMU_CLASS_USE_INTERFACE_DESCRIPTOR = 0x00, /*< nick=UseInterfaceDescriptor >*/
  USBEMU_CLASS_AUDIO = 0x01, /*< nick=Audio >*/
  USBEMU_CLASS_COMMUNICATIONS_AND_CDC_CONTROL = 0x02, /*< nick=CommunicationsAndCDCControl >*/
  USBEMU_CLASS_HID = 0x03, /*< nick=HID >*/
  USBEMU_CLASS_PHYSICAL = 0x05, /*< nick=Physical >*/
  USBEMU_CLASS_IMAGE = 0x06, /*< nick=Image >*/
  USBEMU_CLASS_PRINTER = 0x07, /*< nick=Printer >*/
  USBEMU_CLASS_MASS_STORAGE = 0x08, /*< nick=MassStorage >*/
  USBEMU_CLASS_HUB = 0x09, /*< nick=Hub >*/
  USBEMU_CLASS_CDC_DATA = 0x0A, /*< nick=CDCData >*/
  USBEMU_CLASS_SMART_CARD = 0x0B, /*< nick=SmartCard >*/
  USBEMU_CLASS_CONTENT_SECURITY = 0x0D, /*< nick=ContentSecurity >*/
  USBEMU_CLASS_VIDEO = 0x0E, /*< nick=Video >*/
  USBEMU_CLASS_PERSONAL_HEALTHCARE = 0x0F, /*< nick=PersonalHealthcare >*/
  USBEMU_CLASS_AUDIO_VIDEO = 0x10, /*< nick=AudioVideo >*/
  USBEMU_CLASS_BILLBOARD = 0x11, /*< nick=Billboard >*/
  USBEMU_CLASS_USB_TYPE_C_BRIDGE = 0x12, /*< nick=USBTypeCBridge >*/
  USBEMU_CLASS_DIAGNOSTIC = 0xDC, /*< nick=Diagnostic >*/
  USBEMU_CLASS_WIRELESS_CONTROLLER = 0xE0, /*< nick=WirelessController >*/
  USBEMU_CLASS_MISCELLANEOUS = 0xEF, /*< nick=Miscellaneous >*/
  USBEMU_CLASS_APPLICATION_SPECIFIC = 0xFE, /*< nick=ApplicationSpecific >*/
  USBEMU_CLASS_VENDOR_SPECIFIC = 0xFF, /*< nick=VendorSpecific >*/
} UsbemuClasses;

/**
 * USBEMU_SUB_CLASS_USE_INTERFACE_DESCRIPTOR:
 *
 * Use sub-class information in the Interface Descriptors. This is a descriptive
 * label in usbemu_device_set_sub_class().
 */
#define USBEMU_SUB_CLASS_USE_INTERFACE_DESCRIPTOR 0x00
/**
 * USBEMU_PROTOCOL_USE_INTERFACE_DESCRIPTOR:
 *
 * Use protocol information in the Interface Descriptors. This is a descriptive
 * label in usbemu_device_set_protocol().
 */
#define USBEMU_PROTOCOL_USE_INTERFACE_DESCRIPTOR 0x00
/**
 * USBEMU_PROTOCOL_VENDOR_SPECIFIC:
 *
 * Vendor specific. This is a descriptive label in usbemu_device_set_protocol().
 */
#define USBEMU_PROTOCOL_VENDOR_SPECIFIC 0xFF

/**
 * UsbemuSpeeds:
 * @USBEMU_SPEED_UNKNOWN: unknown or not attached yet
 * @USBEMU_SPEED_LOW: USB 1.1 low speed
 * @USBEMU_SPEED_FULL: USB 1.1 full speed
 * @USBEMU_SPEED_HIGH: USB 2.0 high speed
 * @USBEMU_SPEED_WIRELESS: USB wireless
 * @USBEMU_SPEED_SUPER: USB 3.0 super speed
 * @USBEMU_SPEED_SUPER_PLUS: USB 3.1 super+ speed
 *
 * USB device/bus speed.
 */
typedef enum /*< enum,prefix=USBEMU >*/
{
  USBEMU_SPEED_UNKNOWN, /*< nick=unknown >*/
  USBEMU_SPEED_LOW, /*< nick=low >*/
  USBEMU_SPEED_FULL, /*< nick=full >*/
  USBEMU_SPEED_HIGH, /*< nick=high >*/
  USBEMU_SPEED_WIRELESS, /*< nick=wireless >*/
  USBEMU_SPEED_SUPER, /*< nick=super >*/
  USBEMU_SPEED_SUPER_PLUS, /*< nick=super_plus >*/
} UsbemuSpeeds;

UsbemuDevice* usbemu_device_new             ();
UsbemuDevice* usbemu_device_new_from_argv   (gchar       **argv,
                                             GError      **error);
UsbemuDevice* usbemu_device_new_from_string (const gchar  *str,
                                             GError      **error);

gboolean usbemu_device_get_attached  (UsbemuDevice         *device);
void     usbemu_device_attach        (UsbemuDevice         *device,
                                      gchar               **options,
                                      GCancellable         *cancellable,
                                      GAsyncReadyCallback   callback,
                                      gpointer              user_data);
gboolean usbemu_device_attach_finish (UsbemuDevice         *device,
                                      GAsyncResult         *result,
                                      GError              **error);
void     usbemu_device_detach        (UsbemuDevice         *device,
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

gboolean                     usbemu_device_add_configuration    (UsbemuDevice                *device,
                                                                 struct _UsbemuConfiguration *configuration);
struct _UsbemuConfiguration* usbemu_device_get_configuration    (UsbemuDevice                *device,
                                                                 guint                        configuration_value);
GSList*                      usbemu_device_get_configurations   (UsbemuDevice                *device);
guint                        usbemu_device_get_n_configurations (UsbemuDevice                *device);

G_END_DECLS
