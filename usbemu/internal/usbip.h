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

#if !defined (LIBUSBEMU_COMPILATION)
#error "libusbemu internal use only."
#endif

#include <glib.h>

#include <usbemu/usbemu-interface.h>
#include <usbemu/usbemu-urb.h>

#ifndef __GTK_DOC_IGNORE__

/* BEGIN SHAMELESS COPY FROM <KERNEL>/drivers/usb/usbip/usbip_common.h */

#define USBIP_CMD_SUBMIT  0x0001
#define USBIP_CMD_UNLINK  0x0002
#define USBIP_RET_SUBMIT  0x0003
#define USBIP_RET_UNLINK  0x0004

#define USBIP_DIR_OUT  0x00
#define USBIP_DIR_IN  0x01

struct usbip_header_basic {
  guint32 command;
  guint32 seqnum;
  guint32 devid;
  guint32 direction;
  guint32 ep;
} __attribute__ ((packed));

struct usbip_header_cmd_submit {
  guint32 transfer_flags;
  gint32  transfer_buffer_length;

  gint32  start_frame;
  gint32  number_of_packets;
  gint32  interval;

  guchar  setup[8];
} __attribute__ ((packed));

struct usbip_header_ret_submit {
  gint32 status;
  gint32 actual_length;
  gint32 start_frame;
  gint32 number_of_packets;
  gint32 error_count;
} __attribute__ ((packed));

struct usbip_header_cmd_unlink {
  guint32 seqnum;
} __attribute__ ((packed));

struct usbip_header_ret_unlink {
  gint32 status;
} __attribute__ ((packed));

struct usbip_header {
  struct usbip_header_basic base;

  union {
    struct usbip_header_cmd_submit cmd_submit;
    struct usbip_header_ret_submit ret_submit;
    struct usbip_header_cmd_unlink cmd_unlink;
    struct usbip_header_ret_unlink ret_unlink;
  } u;
} __attribute__ ((packed));

struct usbip_iso_packet_descriptor {
  guint32 offset;
  guint32 length;
  guint32 actual_length;
  guint32 status;
} __attribute__ ((packed));

/* END SHAMELESS COPY FROM <KERNEL>/drivers/usb/usbip/usbip_common.h */

void _usbemu_usbip_header_correct_endian     (struct usbip_header                *header,
                                              UsbemuEndpointDirections            direction);
void _usbemu_usbip_iso_packet_correct_endian (struct usbip_iso_packet_descriptor *iso_frame_desc,
                                              UsbemuEndpointDirections            direction);
void _usbemu_usbip_header_dump               (struct usbip_header                *header);
void _usbemu_usbip_cmd_submit_header_to_urb  (struct usbip_header_cmd_submit     *cmd_submit,
                                              UsbemuUrb                          *urb);
void _usbemu_usbip_iso_packet_to_urb         (struct usbip_iso_packet_descriptor *iso_frame_desc,
                                              UsbemuIsoPacketDescriptor          *descriptor);

#endif /* !__GTK_DOC_IGNORE__ */
