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
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _UsbemuIsoPacketDescriptor
{
  guint32 offset;
  guint32 length;
  guint32 actual_length;
  guint32 status;
} UsbemuIsoPacketDescriptor;

typedef struct _UsbemuUrb
{
  guint32 transfer_flags;
  gint32 transfer_buffer_length;
  gpointer transfer_buffer;
  gint32 start_frame;
  gint32 number_of_packets;
  UsbemuIsoPacketDescriptor *iso_frame_desc;
  gint32 interval;
  gchar setup[8];
} UsbemuUrb;

UsbemuUrb* usbemu_urb_new   ();
UsbemuUrb* usbemu_urb_ref   (UsbemuUrb *urb);
void       usbemu_urb_unref (UsbemuUrb *urb);

G_END_DECLS
