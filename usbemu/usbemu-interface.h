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

#include <usbemu/usbemu-configuration.h>

G_BEGIN_DECLS

/**
 * USBEMU_TYPE_INTERFACE:
 *
 * Convenient macro for usbemu_interface_get_type().
 */
#define USBEMU_TYPE_INTERFACE  (usbemu_interface_get_type ())

G_DECLARE_DERIVABLE_TYPE (UsbemuInterface, usbemu_interface, USBEMU, INTERFACE,
                          GObject)

/**
 * USBEMU_INTERFACE_PROP_INTERFACE_NUMBER:
 *
 * "interface-number" property name.
 */
#define USBEMU_INTERFACE_PROP_INTERFACE_NUMBER "interface-number"
/**
 * USBEMU_INTERFACE_PROP_ALTERNATE_SETTING:
 *
 * "alternate-setting" property name.
 */
#define USBEMU_INTERFACE_PROP_ALTERNATE_SETTING "alternate-setting"
/**
 * USBEMU_INTERFACE_PROP_NAME:
 *
 * "name" property name.
 */
#define USBEMU_INTERFACE_PROP_NAME "name"
/**
 * USBEMU_INTERFACE_PROP_CLASS:
 *
 * "class" property name.
 */
#define USBEMU_INTERFACE_PROP_CLASS "class"
/**
 * USBEMU_INTERFACE_PROP_SUB_CLASS:
 *
 * "sub-class" property name.
 */
#define USBEMU_INTERFACE_PROP_SUB_CLASS "sub-class"
/**
 * USBEMU_INTERFACE_PROP_PROTOCOL:
 *
 * "protocol" property name.
 */
#define USBEMU_INTERFACE_PROP_PROTOCOL "protocol"

struct _UsbemuInterfaceClass {
  GObjectClass parent_class;

  /*< private >*/

  /* Reserved slots for furture extension. */
  gpointer padding[12];
};

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
 * UsbemuEndpointEntry:
 * @endpoint_number: a #UsbemuEndpoints. Control endpoints are automatically
 *     included, so should never be used here.
 * @direction: a #UsbemuEndpointDirections. Ignored for control endpoints.
 * @transfer: a #UsbemuEndpointTransfers.
 * @attributes: Bit-wise OR-ed combination of #UsbemuEndpointIsochronousSyncs
 *     and #UsbemuEndpointIsochronousUsages for isochronous endpoints. Must be
 *     zero otherwise.
 * @max_packet_size: maximum packet size in bytes.
 * @additional_transactions: for high-speed isochronous and interrupt endpoints,
 *     this specifies the number of additional transaction opportunities per
 *     microframe. Valid values are 0, 1 and 2.
 * @interval: Interval for polling endpoint for data transfers in µs.
 *
 * Per endpoint definition entry for #UsbemuInterface. See
 * usbemu_interface_add_endpoint_entries().
 */
typedef struct {
  UsbemuEndpoints endpoint_number;
  UsbemuEndpointDirections direction;
  UsbemuEndpointTransfers transfer;
  guint8 attributes;
  guint max_packet_size;
  guint additional_transactions;
  guint interval;
} UsbemuEndpointEntry;

UsbemuInterface* usbemu_interface_new             ();
UsbemuInterface* usbemu_interface_new_full        (const gchar   *name,
                                                   UsbemuClasses  klass,
                                                   guint          sub_class,
                                                   guint          protocol);
UsbemuInterface* usbemu_interface_new_from_argv   (gchar       **argv,
                                                   GError      **error);
UsbemuInterface* usbemu_interface_new_from_string (const gchar  *str,
                                                   GError      **error);

guint         usbemu_interface_get_interface_number  (UsbemuInterface *interface);
guint         usbemu_interface_get_alternate_setting (UsbemuInterface *interface);
const gchar*  usbemu_interface_get_name              (UsbemuInterface *interface);
void          usbemu_interface_set_name              (UsbemuInterface *interface,
                                                      const gchar     *name);
UsbemuClasses usbemu_interface_get_class             (UsbemuInterface *interface);
void          usbemu_interface_set_class             (UsbemuInterface *interface,
                                                      UsbemuClasses    klass);
guint         usbemu_interface_get_sub_class         (UsbemuInterface *interface);
void          usbemu_interface_set_sub_class         (UsbemuInterface *interface,
                                                      guint            sub_class);
guint         usbemu_interface_get_protocol          (UsbemuInterface *interface);
void          usbemu_interface_set_protocol          (UsbemuInterface *interface,
                                                      guint            protocol);

void                       usbemu_interface_add_endpoint_entries (UsbemuInterface           *interface,
                                                                  const UsbemuEndpointEntry *entries);
const UsbemuEndpointEntry* usbemu_interface_get_endpoint_entries (UsbemuInterface           *interface);

UsbemuConfiguration* usbemu_interface_get_configuration (UsbemuInterface *interface);

G_END_DECLS
