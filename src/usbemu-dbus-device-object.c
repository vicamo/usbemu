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

#include "config.h"

#include <usbemu/dbus/usbemu-dbus-device.h>

#include "usbemu-dbus-device-object.h"

/**
 * UsbemuDBusDeviceObject:
 *
 * Service-side D-Bus device object.
 */

/**
 * UsbemuDBusDeviceObjectClass:
 * @parent_class: The parent class.
 *
 * Class structure for UsbemuDBusDeviceObject.
 */

typedef struct  _UsbemuDBusDeviceObjectPrivate {
  UsbemuDBusDeviceSkeleton *device_skeleton;

  UsbemuDevice *device;
} UsbemuDBusDeviceObjectPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (UsbemuDBusDeviceObject, usbemu_dbus_device_object,
                            G_TYPE_DBUS_OBJECT_SKELETON)

#define USBEMU_DBUS_DEVICE_OBJECT_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), USBEMU_TYPE_DBUS_DEVICE_OBJECT, \
                                UsbemuDBusDeviceObjectPrivate))

enum
{
  PROP_0,
  PROP_DEVICE,
  N_PROPERTIES
};

static GParamSpec *props[N_PROPERTIES] = { NULL, };

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
  UsbemuDBusDeviceObject *device = USBEMU_DBUS_DEVICE_OBJECT (object);
  UsbemuDBusDeviceObjectPrivate *priv =
    USBEMU_DBUS_DEVICE_OBJECT_GET_PRIVATE (device);

  switch (prop_id) {
    case PROP_DEVICE:
      g_assert (priv->device == NULL);
      priv->device = g_value_dup_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
  UsbemuDBusDeviceObject *device = USBEMU_DBUS_DEVICE_OBJECT (object);

  switch (prop_id) {
    case PROP_DEVICE:
      g_value_set_object (value, usbemu_dbus_device_object_get_device (device));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
constructed (GObject *object)
{
  UsbemuDBusDeviceObject *device = USBEMU_DBUS_DEVICE_OBJECT (object);
  UsbemuDBusDeviceObjectPrivate *priv =
    USBEMU_DBUS_DEVICE_OBJECT_GET_PRIVATE (device);
  UsbemuDBusDeviceSkeleton *device_skeleton;

  G_OBJECT_CLASS (usbemu_dbus_device_object_parent_class)->constructed (object);

  /* setup UsbemuDBusDeviceSkeleton */
  priv->device_skeleton = device_skeleton =
    (UsbemuDBusDeviceSkeleton*) usbemu_dbus_device_skeleton_new ();
  g_object_ref (priv->device_skeleton);

  usbemu_dbus_device_set_udi (USBEMU_DBUS_DEVICE (device_skeleton),
                              usbemu_device_get_udi (priv->device));
  g_dbus_object_skeleton_add_interface (G_DBUS_OBJECT_SKELETON (device),
                                        G_DBUS_INTERFACE_SKELETON (device_skeleton));
}

static void
dispose (GObject *object)
{
  UsbemuDBusDeviceObject *device = USBEMU_DBUS_DEVICE_OBJECT (object);
  UsbemuDBusDeviceObjectPrivate *priv =
    USBEMU_DBUS_DEVICE_OBJECT_GET_PRIVATE (device);

  g_dbus_object_skeleton_remove_interface (G_DBUS_OBJECT_SKELETON (object),
                                           G_DBUS_INTERFACE_SKELETON (priv->device_skeleton));
  g_object_unref (priv->device_skeleton);
  priv->device_skeleton = NULL;

  g_object_unref (priv->device);
  priv->device = NULL;
}

static void
usbemu_dbus_device_object_class_init (UsbemuDBusDeviceObjectClass *device_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (device_class);

  /* virtual methods */

  object_class->constructed = constructed;
  object_class->set_property = set_property;
  object_class->get_property = get_property;
  object_class->dispose = dispose;

  /* properties */

  /**
   * UsbemuDBusDeviceObject:device:
   *
   * Underlying lib device object.
   */
  props[PROP_DEVICE] =
    g_param_spec_object (USBEMU_DBUS_DEVICE_OBJECT_PROP_DEVICE,
                         "Device", "Device",
                         USBEMU_TYPE_DEVICE,
                         G_PARAM_READWRITE | \
                           G_PARAM_CONSTRUCT_ONLY | \
                           G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPERTIES, props);
}

static void
usbemu_dbus_device_object_init (UsbemuDBusDeviceObject *device G_GNUC_UNUSED)
{
  /* do nothing */
}

UsbemuDBusDeviceObject*
usbemu_dbus_device_object_new (const gchar  *object_path,
                               UsbemuDevice *device)
{
  g_return_val_if_fail (g_variant_is_object_path (object_path), NULL);
  g_return_val_if_fail (USBEMU_IS_DEVICE (device), NULL);

  return g_object_new (USBEMU_TYPE_DBUS_DEVICE_OBJECT,
                       "g-object-path", object_path,
                       USBEMU_DBUS_DEVICE_OBJECT_PROP_DEVICE, device,
                       NULL);
}

UsbemuDevice*
usbemu_dbus_device_object_get_device (UsbemuDBusDeviceObject *device)
{
  UsbemuDBusDeviceObjectPrivate *priv;

  g_return_val_if_fail (USBEMU_IS_DBUS_DEVICE_OBJECT (device), NULL);

  priv = USBEMU_DBUS_DEVICE_OBJECT_GET_PRIVATE (device);
  return priv->device;
}

UsbemuDevice*
usbemu_dbus_device_object_dup_device (UsbemuDBusDeviceObject *device)
{
  return g_object_ref (usbemu_dbus_device_object_get_device(device));
}