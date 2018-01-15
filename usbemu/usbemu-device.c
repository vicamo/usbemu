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

#if defined (HAVE_CONFIG_H)
#include "config.h"
#endif

#include "usbemu/usbemu-device.h"

/**
 * UsbemuDevice:
 *
 * An object representing emulated USB device.
 */

/**
 * UsbemuDeviceClass:
 * @parent_class: The parent class.
 *
 * Class structure for UsbemuDevice.
 */

typedef struct  _UsbemuDevicePrivate {
} UsbemuDevicePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (UsbemuDevice, usbemu_device, G_TYPE_OBJECT)

#define USBEMU_DEVICE_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), USBEMU_TYPE_DEVICE, UsbemuDevicePrivate))

enum
{
  PROP_0,
  N_PROPERTIES
};

static GParamSpec *props[N_PROPERTIES] = { NULL, };

/* virtual methods for GObjectClass */
static void gobject_class_set_property (GObject *object, guint prop_id,
                                        const GValue *value, GParamSpec *pspec);
static void gobject_class_get_property (GObject *object, guint prop_id,
                                        GValue *value, GParamSpec *pspec);
static void gobject_class_finalize (GObject *object);
/* virtual methods for UsbemuDeviceClass */
static void usbemu_device_class_init (UsbemuDeviceClass *device_class);

static void
gobject_class_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  UsbemuDevice *device = USBEMU_DEVICE (object);
  UsbemuDevicePrivate *priv = USBEMU_DEVICE_GET_PRIVATE (device);

  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gobject_class_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  UsbemuDevice *device = USBEMU_DEVICE (object);

  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gobject_class_finalize (GObject *object G_GNUC_UNUSED)
{
}

static void
usbemu_device_class_init (UsbemuDeviceClass *device_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (device_class);

  /* virtual methods */

  object_class->set_property = gobject_class_set_property;
  object_class->get_property = gobject_class_get_property;
  object_class->finalize = gobject_class_finalize;

  /* properties */

  g_object_class_install_properties (object_class, N_PROPERTIES, props);
}

static void
usbemu_device_init (UsbemuDevice *device G_GNUC_UNUSED)
{
  /* do nothing */
}

/**
 * usbemu_device_new:
 *
 * Returns: (transfer full) (type UsbemuDevice): The constructed device object
 *          or %NULL.
 */
UsbemuDevice*
usbemu_device_new ()
{
  return g_object_new (USBEMU_TYPE_DEVICE, NULL);
}
