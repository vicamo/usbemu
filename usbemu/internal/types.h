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
#include <gio/gio.h>

#include "usbemu/usbemu-urb.h"

/**
 * SECTION:types
 * @short_description: private structures
 * @stability: private
 *
 * Internally used structures.
 */

#ifndef __GTK_DOC_IGNORE__

typedef struct _UsbemuUrbReal
{
  UsbemuUrb urb;

  gint ref_count;
  GTask *task;
} UsbemuUrbReal;

#endif /* !__GTK_DOC_IGNORE__ */
