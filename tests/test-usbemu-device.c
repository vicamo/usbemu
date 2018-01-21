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

#include <locale.h>
#include <glib.h>

#include "usbemu/usbemu.h"

static void
test_instanciation_new_1 (void)
{
  UsbemuDevice *device;

  device = usbemu_device_new ();
  g_assert_nonnull (device);
  g_assert_true (USBEMU_IS_DEVICE (device));

  g_object_unref (device);
}

static void
test_instanciation_new_from_string_1 (void)
{
  const gchar* strings[] = {
    NULL,
    "",
    "object-type=UsbemuDevice",
    "--configuration",
    "object-type=UsbemuDevice --configuration",
    "--configuration --configuration",
    "--configuration --interface",
    "--configuration --interface --interface",
    "--configuration --interface --configuration",
    "--configuration --interface --configuration --interface",
  };
  UsbemuDevice *device;
  GError *error = NULL;
  gsize i;

  for (i = 0; i < G_N_ELEMENTS (strings); i++) {
    device = usbemu_device_new_from_string (strings[i], &error);
    g_assert_nonnull (device);
    g_assert_true (USBEMU_IS_DEVICE (device));
    g_assert_null (error);
    g_object_unref (device);
  }
}

static void
test_instanciation_new_from_string_2 (void)
{
  const struct {
    gchar *description;
    GQuark error_domain;
    gint error_code;
  } cases[] = {
    { "object-type=NoSuchType",
      USBEMU_ERROR, USBEMU_ERROR_INVALID_TYPE },
    { "object-type=UsbemuDevice name",
      USBEMU_ERROR, USBEMU_ERROR_SYNTAX_ERROR },
    { "object-type=UsbemuDevice name=",
      USBEMU_ERROR, USBEMU_ERROR_SYNTAX_ERROR },
    { "object-type=UsbemuDevice =value",
      USBEMU_ERROR, USBEMU_ERROR_SYNTAX_ERROR },
    { "object-type=UsbemuDevice name=value",
      USBEMU_ERROR, USBEMU_ERROR_INVALID_TYPE },
  };
  UsbemuDevice *device;
  GError *error = NULL;
  gsize i;

  for (i = 0; i < G_N_ELEMENTS (cases); i++) {
    g_test_message ("testing: %s", cases[i].description);
    device = usbemu_device_new_from_string (cases[i].description, &error);
    g_assert_null (device);
    if (cases[i].error_domain == 0)
      g_assert_null (error);
    else {
      g_assert_nonnull (error);
      g_test_message ("got error %s:%d",
                      g_quark_to_string (error->domain), error->code);
      g_assert_cmpint (error->domain, ==, cases[i].error_domain);
      g_assert_cmpint (error->code, ==, cases[i].error_code);
      g_error_free (error);
      error = NULL;
    }
  }
}

static void
test_properties_specification_num_1 (void)
{
  UsbemuDevice *device;

  device = usbemu_device_new ();
  g_test_queue_unref (device);

  /* default specification number is 0x100. */
  g_assert_cmpint (usbemu_device_get_specification_num (device), ==, 0x100);

  usbemu_device_set_specification_num (device, 0x210);
  g_assert_cmpint (usbemu_device_get_specification_num (device), ==, 0x210);
}

static void
test_properties_class_1 (void)
{
  const UsbemuClasses classes[] = {
    USBEMU_CLASS_AUDIO, USBEMU_CLASS_COMMUNICATIONS_AND_CDC_CONTROL,
    USBEMU_CLASS_HID, USBEMU_CLASS_PHYSICAL, USBEMU_CLASS_IMAGE,
    USBEMU_CLASS_PRINTER, USBEMU_CLASS_MASS_STORAGE, USBEMU_CLASS_HUB,
    USBEMU_CLASS_CDC_DATA, USBEMU_CLASS_SMART_CARD,
    USBEMU_CLASS_CONTENT_SECURITY, USBEMU_CLASS_VIDEO,
    USBEMU_CLASS_PERSONAL_HEALTHCARE, USBEMU_CLASS_AUDIO_VIDEO,
    USBEMU_CLASS_BILLBOARD, USBEMU_CLASS_USB_TYPE_C_BRIDGE,
    USBEMU_CLASS_DIAGNOSTIC, USBEMU_CLASS_WIRELESS_CONTROLLER,
    USBEMU_CLASS_MISCELLANEOUS, USBEMU_CLASS_APPLICATION_SPECIFIC,
    USBEMU_CLASS_VENDOR_SPECIFIC, USBEMU_CLASS_USE_INTERFACE_DESCRIPTOR,
  };
  UsbemuDevice *device;
  gsize i;

  device = usbemu_device_new ();
  g_test_queue_unref (device);

  /* default class code is USBEMU_CLASS_USE_INTERFACE_DESCRIPTOR. */
  g_assert_cmpint (usbemu_device_get_class (device), ==,
                   USBEMU_CLASS_USE_INTERFACE_DESCRIPTOR);

  for (i = 0; i < G_N_ELEMENTS (classes); i++) {
    usbemu_device_set_class (device, classes[i]);
    g_assert_cmpint (usbemu_device_get_class (device), ==, classes[i]);
  }
}

static void
test_properties_sub_class_1 (void)
{
  guint sub_classes[] = {
    0x01, 0x02, USBEMU_SUB_CLASS_USE_INTERFACE_DESCRIPTOR,
  };
  UsbemuDevice *device;
  gsize i;

  device = usbemu_device_new ();
  g_test_queue_unref (device);

  /* default sub-class code is USBEMU_SUB_CLASS_USE_INTERFACE_DESCRIPTOR. */
  g_assert_cmpint (usbemu_device_get_sub_class (device), ==,
                   USBEMU_SUB_CLASS_USE_INTERFACE_DESCRIPTOR);

  for (i = 0; i < G_N_ELEMENTS (sub_classes); i++) {
    usbemu_device_set_sub_class (device, sub_classes[i]);
    g_assert_cmpint (usbemu_device_get_sub_class (device), ==, sub_classes[i]);
  }
}

static void
test_properties_protocol_1 (void)
{
  guint protocols[] = {
    0x01, 0x02, USBEMU_PROTOCOL_VENDOR_SPECIFIC,
    USBEMU_SUB_CLASS_USE_INTERFACE_DESCRIPTOR,
  };
  UsbemuDevice *device;
  gsize i;

  device = usbemu_device_new ();
  g_test_queue_unref (device);

  /* default protocol code is USBEMU_PROTOCOL_USE_INTERFACE_DESCRIPTOR. */
  g_assert_cmpint (usbemu_device_get_protocol (device), ==,
                   USBEMU_PROTOCOL_USE_INTERFACE_DESCRIPTOR);

  for (i = 0; i < G_N_ELEMENTS (protocols); i++) {
    usbemu_device_set_protocol (device, protocols[i]);
    g_assert_cmpint (usbemu_device_get_protocol (device), ==, protocols[i]);
  }
}

static void
test_properties_max_packet_size_1 (void)
{
  UsbemuDevice *device;

  device = usbemu_device_new ();
  g_test_queue_unref (device);

  /* default max packet size is 0. */
  g_assert_cmpint (usbemu_device_get_max_packet_size (device), ==, 0);

  usbemu_device_set_max_packet_size (device, 0xFF);
  g_assert_cmpint (usbemu_device_get_max_packet_size (device), ==, 0xFF);
}

static void
test_properties_vendor_id_1 (void)
{
  UsbemuDevice *device;

  device = usbemu_device_new ();
  g_test_queue_unref (device);

  /* default vendor id is 0xdead. */
  g_assert_cmpint (usbemu_device_get_vendor_id (device), ==, 0xdead);

  usbemu_device_set_vendor_id (device, 0x210);
  g_assert_cmpint (usbemu_device_get_vendor_id (device), ==, 0x210);
}

static void
test_properties_product_id_1 (void)
{
  UsbemuDevice *device;

  device = usbemu_device_new ();
  g_test_queue_unref (device);

  /* default product id is 0xbeef. */
  g_assert_cmpint (usbemu_device_get_product_id (device), ==, 0xbeef);

  usbemu_device_set_product_id (device, 0x210);
  g_assert_cmpint (usbemu_device_get_product_id (device), ==, 0x210);
}

static void
test_properties_release_number_1 (void)
{
  UsbemuDevice *device;

  device = usbemu_device_new ();
  g_test_queue_unref (device);

  /* default release number is 0x100. */
  g_assert_cmpint (usbemu_device_get_release_number (device), ==, 0x100);

  usbemu_device_set_release_number (device, 0x210);
  g_assert_cmpint (usbemu_device_get_release_number (device), ==, 0x210);
}

static void
test_properties_manufacturer_name_1 (void)
{
  const gchar *names[] = {
    NULL, "example.com",
  };
  UsbemuDevice *device;
  gsize i;

  device = usbemu_device_new ();
  g_test_queue_unref (device);

  /* default manufacturer name is PACKAGE_NAME. */
  g_assert_cmpstr (usbemu_device_get_manufacturer_name (device), ==,
                   PACKAGE_NAME);

  for (i = 0; i < G_N_ELEMENTS (names); i++) {
    usbemu_device_set_manufacturer_name (device, names[i]);
    if (names[i] == NULL)
      g_assert_null (usbemu_device_get_manufacturer_name (device));
    else
      g_assert_cmpstr (usbemu_device_get_manufacturer_name (device), ==,
                       names[i]);
  }
}

static void
test_properties_product_name_1 (void)
{
  const gchar *names[] = {
    NULL, "device 1",
  };
  UsbemuDevice *device;
  gsize i;

  device = usbemu_device_new ();
  g_test_queue_unref (device);

  /* default product name is "emulated device". */
  g_assert_cmpstr (usbemu_device_get_product_name (device), ==,
                   "emulated device");

  for (i = 0; i < G_N_ELEMENTS (names); i++) {
    usbemu_device_set_product_name (device, names[i]);
    if (names[i] == NULL)
      g_assert_null (usbemu_device_get_product_name (device));
    else
      g_assert_cmpstr (usbemu_device_get_product_name (device), ==, names[i]);
  }
}

static void
test_properties_serial_1 (void)
{
  const gchar *serials[] = {
    NULL, "1",
  };
  UsbemuDevice *device;
  gsize i;

  device = usbemu_device_new ();
  g_test_queue_unref (device);

  /* default serial number is "9641c4a0c0d26686a3fcdc92711f8f42". */
  g_assert_cmpstr (usbemu_device_get_serial (device), ==,
                   "9641c4a0c0d26686a3fcdc92711f8f42");

  for (i = 0; i < G_N_ELEMENTS (serials); i++) {
    usbemu_device_set_serial (device, serials[i]);
    if (serials[i] == NULL)
      g_assert_null (usbemu_device_get_serial (device));
    else
      g_assert_cmpstr (usbemu_device_get_serial (device), ==, serials[i]);
  }
}

int
main (int   argc,
      char *argv[])
{
  setlocale (LC_ALL, "");

  g_test_init (&argc, &argv, NULL);
  g_test_bug_base (PACKAGE_BUGREPORT);

  /* instanciation */

  g_test_add_func ("/UsbemuDevice/instanciation/new",
                   test_instanciation_new_1);
  g_test_add_func ("/UsbemuDevice/instanciation/new-from-string/1",
                   test_instanciation_new_from_string_1);
  g_test_add_func ("/UsbemuDevice/instanciation/new-from-string/2",
                   test_instanciation_new_from_string_2);

  /* properties */

  /* specification-num */
  g_test_add_func ("/UsbemuDevice/properties/specification-num",
                   test_properties_specification_num_1);
  /* class code */
  g_test_add_func ("/UsbemuDevice/properties/class",
                   test_properties_class_1);
  /* sub-class code */
  g_test_add_func ("/UsbemuDevice/properties/sub-class",
                   test_properties_sub_class_1);
  /* protocol code */
  g_test_add_func ("/UsbemuDevice/properties/protocol",
                   test_properties_protocol_1);
  /* max-packet-size */
  g_test_add_func ("/UsbemuDevice/properties/max-packet-size",
                   test_properties_max_packet_size_1);
  /* vendor-id */
  g_test_add_func ("/UsbemuDevice/properties/vendor-id",
                   test_properties_vendor_id_1);
  /* product-id */
  g_test_add_func ("/UsbemuDevice/properties/product-id",
                   test_properties_product_id_1);
  /* release-number */
  g_test_add_func ("/UsbemuDevice/properties/release-number",
                   test_properties_release_number_1);
  /* manufacturer name */
  g_test_add_func ("/UsbemuDevice/properties/manufacturer-name",
                   test_properties_manufacturer_name_1);
  /* product name */
  g_test_add_func ("/UsbemuDevice/properties/product-name",
                   test_properties_product_name_1);
  /* serial */
  g_test_add_func ("/UsbemuDevice/properties/serial",
                   test_properties_serial_1);

  return g_test_run ();
}
