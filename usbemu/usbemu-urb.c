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

#include "usbemu/usbemu-urb.h"

#include "usbemu/internal/types.h"

/**
 * SECTION:usbemu-urb
 * @title: UsbemuUrb
 * @short_description: USBEmu Request Block
 * @include: usbemu/usbemu.h
 *
 * TODO.
 */

/**
 * UsbemuUrb:
 * @transfer_flags: Linux kernel urb->transfer_flags. Currently not used.
 * @transfer_buffer_length: size of @transfer_buffer.
 * @transfer_buffer: I/O buffer.
 * @start_frame: the initial frame for isochronous transfers.
 * @number_of_packets: the number of ISO transfer buffers.
 * @iso_frame_desc: Used to provide arrays of ISO transfer buffers and to
 *                  collect the transfer status for each buffer.
 * @interval: Specifies the polling interval for interrupt or isochronous
 *            transfers. The units are frames (milliseconds) for full and low
 *            speed devices, and microframes (1/8 millisecond) for highspeed
 *            and SuperSpeed devices.
 * @setup: Only used for control transfers, this points to eight bytes of setup
 *         data.
 *
 * USB Request Block for USBEmu.
 */

/**
 * UsbemuIsoPacketDescriptor:
 * @offset: TODO.
 * @length: TODO.
 * @actual_length: TODO.
 * @status: TODO.
 *
 * TODO.
 */

/**
 * usbemu_urb_new:
 *
 * Creates a new #UsbemuUrb with a reference count of 1.
 *
 * Returns: (transfer full): the new #UsbemuUrb.
 */
UsbemuUrb*
usbemu_urb_new ()
{
  UsbemuUrbReal *rurb;

  rurb = g_slice_new0 (UsbemuUrbReal);
  rurb->ref_count = 1;
  rurb->task = NULL;
  rurb->urb.transfer_buffer = NULL;
  rurb->urb.iso_frame_desc = NULL;

  return &rurb->urb;
}

/**
 * usbemu_urb_ref:
 * @urb: (in): a #UsbemuUrb.
 *
 * Atomically increments the reference count of @urb by one. This function is
 * thread-safe and may be called from any thread.
 *
 * Returns: (transfer none): the passed #UsbemuUrb.
 */
UsbemuUrb*
usbemu_urb_ref (UsbemuUrb *urb)
{
  UsbemuUrbReal *rurb = (UsbemuUrbReal*)urb;

  g_return_val_if_fail (urb != NULL, NULL);

  g_atomic_int_inc (&rurb->ref_count);

  return urb;
}

/**
 * usbemu_urb_unref:
 * @urb: (in): a #UsbemuUrb.
 *
 * Atomically decrements the reference count of @urb by one. If the reference
 * count drops to 0, all memory allocated by the urb is released. This function
 * is thread-safe and may be called from any thread.
 */
void
usbemu_urb_unref (UsbemuUrb *urb)
{
  UsbemuUrbReal *rurb = (UsbemuUrbReal*)urb;

  g_return_if_fail (urb != NULL);

  if (!g_atomic_int_dec_and_test (&rurb->ref_count))
    return;

  /* task is only used by usbemu_device_submit_urb_async(), and should have
   * been explicitly unref-ed. */
  g_assert_null (rurb->task);

  if (urb->transfer_buffer != NULL)
    g_free (urb->transfer_buffer);
  if (urb->iso_frame_desc != NULL) {
    g_slice_free1 (urb->number_of_packets * sizeof (UsbemuIsoPacketDescriptor),
                   urb->iso_frame_desc);
  }

  g_slice_free1 (sizeof (UsbemuUrbReal), rurb);
}
