/* usbemu - USB Emulation Library
 * Copyright (C) 2016 You-Sheng Yang
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details:
 */

#pragma once

#include <glib-object.h>
#include <gio/gio.h>
#include <usbemu/usbemu.h>

#include "usbemu-dbus-manager-object.h"

G_BEGIN_DECLS

#define USBEMU_TYPE_USBIP_SERVER  (usbemu_usbip_server_get_type ())

G_DECLARE_DERIVABLE_TYPE (UsbemuUsbipServer, usbemu_usbip_server, USBEMU, \
                          USBIP_SERVER, GObject)

#define USBEMU_USBIP_SERVER_PROP_MANAGER "manager"

struct _UsbemuUsbipServerClass {
  GObjectClass parent_class;

  /*< private >*/

  /* Reserved slots for furture extension. */
  gpointer padding[12];
};

UsbemuUsbipServer* usbemu_usbip_server_new (UsbemuDBusManagerObject *manager);

UsbemuDBusManagerObject* usbemu_usbip_server_get_manager (UsbemuUsbipServer *server);
UsbemuDBusManagerObject* usbemu_usbip_server_dup_manager (UsbemuUsbipServer *server);

void usbemu_usbip_server_add_iostream (UsbemuUsbipServer *server,
                                       GIOStream         *iostream);

G_END_DECLS
