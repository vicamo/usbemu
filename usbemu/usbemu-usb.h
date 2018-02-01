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

#include <glib.h>

/**
 * SECTION:usbemu-usb
 * @title: USB Specification Types
 * @short_description: Structures, enumerations for USB specification.
 * @include: usbemu/usbemu.h
 *
 * TODO.
 */

G_BEGIN_DECLS

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
 * UsbemuEndpoints:
 * @USBEMU_EP_CTL: Control endpoint.
 * @USBEMU_EP_1: Endpoint no.1.
 * @USBEMU_EP_2: Endpoint no.2.
 * @USBEMU_EP_3: Endpoint no.3.
 * @USBEMU_EP_4: Endpoint no.4.
 * @USBEMU_EP_5: Endpoint no.5.
 * @USBEMU_EP_6: Endpoint no.6.
 * @USBEMU_EP_7: Endpoint no.7.
 * @USBEMU_EP_8: Endpoint no.8.
 * @USBEMU_EP_9: Endpoint no.9.
 * @USBEMU_EP_10: Endpoint no.10.
 * @USBEMU_EP_11: Endpoint no.11.
 * @USBEMU_EP_12: Endpoint no.12.
 * @USBEMU_EP_13: Endpoint no.13.
 * @USBEMU_EP_14: Endpoint no.14.
 * @USBEMU_EP_15: Endpoint no.15.
 *
 * Predefined endpoint numbers.
 */
typedef enum /*< enum,prefix=USBEMU >*/
{
  USBEMU_EP_CTL = 0, /*< nick=control >*/
  USBEMU_EP_1, /*< nick=ep.1 >*/
  USBEMU_EP_2, /*< nick=ep.2 >*/
  USBEMU_EP_3, /*< nick=ep.3 >*/
  USBEMU_EP_4, /*< nick=ep.4 >*/
  USBEMU_EP_5, /*< nick=ep.5 >*/
  USBEMU_EP_6, /*< nick=ep.6 >*/
  USBEMU_EP_7, /*< nick=ep.7 >*/
  USBEMU_EP_8, /*< nick=ep.8 >*/
  USBEMU_EP_9, /*< nick=ep.9 >*/
  USBEMU_EP_10, /*< nick=ep.10 >*/
  USBEMU_EP_11, /*< nick=ep.11 >*/
  USBEMU_EP_12, /*< nick=ep.12 >*/
  USBEMU_EP_13, /*< nick=ep.13 >*/
  USBEMU_EP_14, /*< nick=ep.14 >*/
  USBEMU_EP_15, /*< nick=ep.15 >*/
} UsbemuEndpoints;

/**
 * USBEMU_NUM_ENDPOINTS:
 *
 * Number of maximum valid endpoints inclusive of control endpoint.
 */
#define USBEMU_NUM_ENDPOINTS (USBEMU_EP_15 + 1)

/**
 * UsbemuEndpointDirections:
 * @USBEMU_ENDPOINT_DIRECTION_OUT: host to device.
 * @USBEMU_ENDPOINT_DIRECTION_IN: device to host.
 *
 * Data flow direction.
 */
typedef enum /*< enum,prefix=USBEMU >*/
{
  USBEMU_ENDPOINT_DIRECTION_OUT = 0x00, /*< nick=out >*/
  USBEMU_ENDPOINT_DIRECTION_IN = 0x80, /*< nick=in >*/
} UsbemuEndpointDirections;

/**
 * UsbemuEndpointTransfers:
 * @USBEMU_ENDPOINT_TRANSFER_CONTROL: Control transfer type. One should not try
 *     to create endpoints of this type, because it's always built-in.
 * @USBEMU_ENDPOINT_TRANSFER_ISOCHRONOUS: Isochronous transfer type.
 * @USBEMU_ENDPOINT_TRANSFER_BULK: Bulk transfer type.
 * @USBEMU_ENDPOINT_TRANSFER_INTERRUPT: Interrupt transfer type.
 *
 * Endpoint transfer type.
 */
typedef enum /*< enum,prefix=USBEMU >*/
{
  USBEMU_ENDPOINT_TRANSFER_CONTROL = 0x00, /*< nick=control >*/
  USBEMU_ENDPOINT_TRANSFER_ISOCHRONOUS = 0x01, /*< nick=isochronous >*/
  USBEMU_ENDPOINT_TRANSFER_BULK = 0x02, /*< nick=bulk >*/
  USBEMU_ENDPOINT_TRANSFER_INTERRUPT = 0x03, /*< nick=interrupt >*/
} UsbemuEndpointTransfers;

/**
 * UsbemuEndpointIsochronousSyncs:
 * @USBEMU_ENDPOINT_ISOCHRONOUS_SYNC_NONE: No synchronization.
 * @USBEMU_ENDPOINT_ISOCHRONOUS_SYNC_ASYNC: Asynchronous.
 * @USBEMU_ENDPOINT_ISOCHRONOUS_SYNC_ADAPTIVE: Adaptive.
 * @USBEMU_ENDPOINT_ISOCHRONOUS_SYNC_SYNC: Synchronous.
 *
 * Synchronization type for isochronous endpoints. For other types of endpoints,
 * use #USBEMU_ENDPOINT_ISOCHRONOUS_SYNC_DONT_CARE.
 */
typedef enum /*< enum,prefix=USBEMU >*/
{
  USBEMU_ENDPOINT_ISOCHRONOUS_SYNC_NONE = 0x00, /*< nick=none >*/
  USBEMU_ENDPOINT_ISOCHRONOUS_SYNC_ASYNC = 0x04, /*< nick=asynchronous >*/
  USBEMU_ENDPOINT_ISOCHRONOUS_SYNC_ADAPTIVE = 0x08, /*< nick=adaptive >*/
  USBEMU_ENDPOINT_ISOCHRONOUS_SYNC_SYNC = 0x0C, /*< nick=synchronous >*/
} UsbemuEndpointIsochronousSyncs;

/**
 * USBEMU_ENDPOINT_ISOCHRONOUS_SYNC_DONT_CARE:
 *
 * Macro used to pack #UsbemuEndpointEntry.attributes.
 */
#define USBEMU_ENDPOINT_ISOCHRONOUS_SYNC_DONT_CARE \
        USBEMU_ENDPOINT_ISOCHRONOUS_SYNC_NONE

/**
 * UsbemuEndpointIsochronousUsages:
 * @USBEMU_ENDPOINT_ISOCHRONOUS_USAGE_DATA: data endpoint.
 * @USBEMU_ENDPOINT_ISOCHRONOUS_USAGE_FEEDBACK: explicit feedback endpoint.
 * @USBEMU_ENDPOINT_ISOCHRONOUS_USAGE_IMPLICIT_FEEDBACK_DATA: implicit feedback
 *     endpoint.
 * @USBEMU_ENDPOINT_ISOCHRONOUS_USAGE_RESERVED: reserved.
 *
 * Usage type for isochronous endpoints. For other types of endpoints, use
 * #USBEMU_ENDPOINT_ISOCHRONOUS_USAGE_DONT_CARE.
 */
typedef enum /*< enum,prefix=USBEMU >*/
{
  USBEMU_ENDPOINT_ISOCHRONOUS_USAGE_DATA = 0x00, /*< nick=data >*/
  USBEMU_ENDPOINT_ISOCHRONOUS_USAGE_FEEDBACK = 0x10, /*< nick=feedback >*/
  USBEMU_ENDPOINT_ISOCHRONOUS_USAGE_IMPLICIT_FEEDBACK_DATA = 0x20, /*< nick=implicit-feedback-data >*/
  USBEMU_ENDPOINT_ISOCHRONOUS_USAGE_RESERVED = 0x30, /*< nick=reserved >*/
} UsbemuEndpointIsochronousUsages;

/**
 * USBEMU_ENDPOINT_ISOCHRONOUS_USAGE_DONT_CARE:
 *
 * Macro used to pack #UsbemuEndpointEntry.attributes.
 */
#define USBEMU_ENDPOINT_ISOCHRONOUS_USAGE_DONT_CARE \
        USBEMU_ENDPOINT_ISOCHRONOUS_USAGE_DATA

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

G_END_DECLS
