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

#if defined (HAVE_CONFIG_H)
#include "config.h"
#endif

#include <string.h>

#include <glib.h>

#include "usbemu/internal/usbip.h"

/**
 * SECTION:usbip
 * @short_description: USBIP protocol structures
 * @stability: private
 *
 * Internally used USBIP protocol structures.
 */

#define LOG_COMPONENT "USBIP: "

static void
_basic_header_correct_endian (struct usbip_header_basic *base,
                              UsbemuEndpointDirections   direction)
{
  if (direction == USBEMU_ENDPOINT_DIRECTION_OUT) {
    base->command = g_ntohl (base->command);
    base->seqnum = g_ntohl (base->seqnum);
    base->devid = g_ntohl (base->devid);
    base->direction = g_ntohl (base->direction);
    base->ep = g_ntohl (base->ep);
  } else {
    base->command = g_htonl (base->command);
    base->seqnum = g_htonl (base->seqnum);
    base->devid = g_htonl (base->devid);
    base->direction = g_htonl (base->direction);
    base->ep = g_htonl (base->ep);
  }
}

static void
_cmd_submit_header_correct_endian (struct usbip_header_cmd_submit *cmd_submit,
                                   UsbemuEndpointDirections        direction)
{
  if (direction == USBEMU_ENDPOINT_DIRECTION_OUT) {
    cmd_submit->transfer_flags = g_ntohl (cmd_submit->transfer_flags);
    cmd_submit->transfer_buffer_length = g_ntohl (cmd_submit->transfer_buffer_length);
    cmd_submit->start_frame = g_ntohl (cmd_submit->start_frame);
    cmd_submit->number_of_packets = g_ntohl (cmd_submit->number_of_packets);
    cmd_submit->interval = g_ntohl (cmd_submit->interval);
  } else {
    cmd_submit->transfer_flags = g_htonl (cmd_submit->transfer_flags);
    cmd_submit->transfer_buffer_length = g_htonl (cmd_submit->transfer_buffer_length);
    cmd_submit->start_frame = g_htonl (cmd_submit->start_frame);
    cmd_submit->number_of_packets = g_htonl (cmd_submit->number_of_packets);
    cmd_submit->interval = g_htonl (cmd_submit->interval);
  }
}

static void
_cmd_unlink_header_correct_endian (struct usbip_header_cmd_unlink *cmd_unlink,
                                   UsbemuEndpointDirections        direction)
{
  if (direction == USBEMU_ENDPOINT_DIRECTION_OUT)
    cmd_unlink->seqnum = g_ntohl (cmd_unlink->seqnum);
  else
    cmd_unlink->seqnum = g_htonl (cmd_unlink->seqnum);
}

static void
_ret_submit_header_correct_endian (struct usbip_header_ret_submit *ret_submit,
                                   UsbemuEndpointDirections        direction)
{
  if (direction == USBEMU_ENDPOINT_DIRECTION_OUT) {
    ret_submit->status = g_ntohl (ret_submit->status);
    ret_submit->actual_length = g_ntohl (ret_submit->actual_length);
    ret_submit->start_frame = g_ntohl (ret_submit->start_frame);
    ret_submit->number_of_packets = g_ntohl (ret_submit->number_of_packets);
    ret_submit->error_count = g_ntohl (ret_submit->error_count);
  } else {
    ret_submit->status = g_htonl (ret_submit->status);
    ret_submit->actual_length = g_htonl (ret_submit->actual_length);
    ret_submit->start_frame = g_htonl (ret_submit->start_frame);
    ret_submit->number_of_packets = g_htonl (ret_submit->number_of_packets);
    ret_submit->error_count = g_htonl (ret_submit->error_count);
  }
}

static void
_ret_unlink_header_correct_endian (struct usbip_header_ret_unlink *ret_unlink,
                                   UsbemuEndpointDirections        direction)
{
  if (direction == USBEMU_ENDPOINT_DIRECTION_OUT)
    ret_unlink->status = g_ntohl (ret_unlink->status);
  else
    ret_unlink->status = g_htonl (ret_unlink->status);
}

G_GNUC_INTERNAL void
_usbemu_usbip_header_correct_endian (struct usbip_header      *header,
                                     UsbemuEndpointDirections  direction)
{
  guint32 command;

  command = (direction == USBEMU_ENDPOINT_DIRECTION_OUT)
        ? g_ntohl (header->base.command) : header->base.command;

  _basic_header_correct_endian (&header->base, direction);

  switch (command) {
    case USBIP_CMD_SUBMIT:
      _cmd_submit_header_correct_endian (&header->u.cmd_submit, direction);
      break;
    case USBIP_CMD_UNLINK:
      _cmd_unlink_header_correct_endian (&header->u.cmd_unlink, direction);
      break;
    case USBIP_RET_SUBMIT:
      _ret_submit_header_correct_endian (&header->u.ret_submit, direction);
      break;
    case USBIP_RET_UNLINK:
      _ret_unlink_header_correct_endian (&header->u.ret_unlink, direction);
      break;
  }
}

G_GNUC_INTERNAL void
_usbemu_usbip_iso_packet_correct_endian (struct usbip_iso_packet_descriptor *iso_frame_desc,
                                         UsbemuEndpointDirections            direction)
{
  if (direction == USBEMU_ENDPOINT_DIRECTION_OUT) {
    iso_frame_desc->offset = g_ntohl (iso_frame_desc->offset);
    iso_frame_desc->length = g_ntohl (iso_frame_desc->length);
    iso_frame_desc->actual_length = g_ntohl (iso_frame_desc->actual_length);
    iso_frame_desc->status = g_ntohl (iso_frame_desc->status);
  } else {
    iso_frame_desc->offset = g_htonl (iso_frame_desc->offset);
    iso_frame_desc->length = g_htonl (iso_frame_desc->length);
    iso_frame_desc->actual_length = g_htonl (iso_frame_desc->actual_length);
    iso_frame_desc->status = g_htonl (iso_frame_desc->status);
  }
}

G_GNUC_INTERNAL void
_usbemu_usbip_header_dump (struct usbip_header *header)
{
  g_debug (LOG_COMPONENT "BASE: cmd %u seq %u devid %u dir %u ep %u",
           header->base.command,
           header->base.seqnum,
           header->base.devid,
           header->base.direction,
           header->base.ep);

  switch (header->base.command) {
    case USBIP_CMD_SUBMIT:
      g_debug (LOG_COMPONENT "USBIP_CMD_SUBMIT: x_flags %u x_len %u sf %u #p %d iv %d",
               header->u.cmd_submit.transfer_flags,
               header->u.cmd_submit.transfer_buffer_length,
               header->u.cmd_submit.start_frame,
               header->u.cmd_submit.number_of_packets,
               header->u.cmd_submit.interval);
      break;
    case USBIP_CMD_UNLINK:
      g_debug (LOG_COMPONENT "USBIP_CMD_UNLINK: seq %u",
               header->u.cmd_unlink.seqnum);
      break;
    case USBIP_RET_SUBMIT:
      g_debug (LOG_COMPONENT "USBIP_RET_SUBMIT: st %d al %u sf %d #p %d ec %d",
               header->u.ret_submit.status,
               header->u.ret_submit.actual_length,
               header->u.ret_submit.start_frame,
               header->u.ret_submit.number_of_packets,
               header->u.ret_submit.error_count);
      break;
    case USBIP_RET_UNLINK:
      g_debug (LOG_COMPONENT "USBIP_RET_UNLINK: status %d",
               header->u.ret_unlink.status);
      break;
  }
}

void
_usbemu_usbip_cmd_submit_header_to_urb (struct usbip_header_cmd_submit *cmd_submit,
                                        UsbemuUrb                      *urb)
{
  urb->transfer_flags = cmd_submit->transfer_flags;
  urb->transfer_buffer_length = cmd_submit->transfer_buffer_length;
  if (urb->transfer_buffer_length != 0)
    urb->transfer_buffer = g_malloc0 (urb->transfer_buffer_length);
  urb->start_frame = cmd_submit->start_frame;
  urb->number_of_packets = cmd_submit->number_of_packets;
  if (urb->number_of_packets != 0) {
    urb->iso_frame_desc =
        g_slice_alloc0 (urb->number_of_packets * sizeof (UsbemuIsoPacketDescriptor));
  }
  urb->interval = cmd_submit->interval;
  memcpy (urb->setup, cmd_submit->setup, sizeof (cmd_submit->setup));
}

void
_usbemu_usbip_iso_packet_to_urb (struct usbip_iso_packet_descriptor *iso_frame_desc,
                                 UsbemuIsoPacketDescriptor          *descriptor)
{
  descriptor->offset = iso_frame_desc->offset;
  descriptor->length = iso_frame_desc->length;
  descriptor->actual_length = iso_frame_desc->actual_length;
  descriptor->status = iso_frame_desc->status;
}
