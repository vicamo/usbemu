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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <gio/gio.h>

#include "usbemu/usbemu-device.h"
#include "usbemu/usbemu-enums.h"
#include "usbemu/usbemu-errors.h"
#include "usbemu/usbemu-urb.h"
#include "usbemu/usbemu-vhci-device.h"

#include "usbemu/internal/usbip.h"
#include "usbemu/internal/utils.h"
#include "usbemu/internal/vhci.h"

/**
 * SECTION:usbemu-vhci-device
 * @short_description: USB VHCI device emulation class
 * @include: usbemu/usbemu.h
 *
 * #UsbemuVhciDevice provides USB emulation for local system.
 */

#define LOG_COMPONENT "VHCI: "

/**
 * UsbemuVhciDevice:
 *
 * An object representing emulated USB device for Linux USB VHCI.
 */

/**
 * UsbemuVhciDeviceClass:
 * @parent_class: the parent class.
 *
 * Class structure for UsbemuVhciDevice.
 */

typedef struct  _UsbemuVhciDevicePrivate {
  guint32 devid;
  guint port;

  GSocketConnection *connection;
  GInputStream *istream;
  GOutputStream *ostream;
  GCancellable *cancellable;

  GList *rx_pending;
  GList *tx_queue;
} UsbemuVhciDevicePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (UsbemuVhciDevice, usbemu_vhci_device,
                            USBEMU_TYPE_DEVICE)

#define USBEMU_VHCI_DEVICE_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), USBEMU_TYPE_VHCI_DEVICE, \
                                UsbemuVhciDevicePrivate))

enum
{
  PROP_0,
  PROP_DEVID,
  PROP_PORT,
  N_PROPERTIES
};

static GParamSpec *props[N_PROPERTIES] = { NULL, };

/* virtual methods for GObjectClass */
static void object_class_set_property (GObject *object, guint prop_id,
                                       const GValue *value, GParamSpec *pspec);
static void object_class_get_property (GObject *object, guint prop_id,
                                       GValue *value, GParamSpec *pspec);
/* virtual methods for UsbemuDeviceClass */
static void device_class_attached (UsbemuDevice *device);
static void device_class_attach_async (UsbemuDevice *device,
                                       GTask *task, gchar **options);
static void device_class_detach_async (UsbemuDevice *device,
                                       GTask *task, gchar **options);
/* init functions */
static void usbemu_vhci_device_class_init (UsbemuVhciDeviceClass *klass);
static void usbemu_vhci_device_init (UsbemuVhciDevice *vhci_device);
/* internal callbacks */
static void _on_istream_fill_callback (GBufferedInputStream *istream,
                                       GAsyncResult         *result,
                                       UsbemuVhciDevice     *vhci_device);
static void _setup_connections_for_attach (GTask *task);

static void
object_class_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  UsbemuVhciDevice *vhci_device = USBEMU_VHCI_DEVICE (object);
  UsbemuVhciDevicePrivate *priv = USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device);

  switch (prop_id) {
    case PROP_DEVID:
      priv->devid = g_value_get_uint (value);
      break;
    case PROP_PORT:
      priv->port = g_value_get_uint (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
object_class_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  UsbemuVhciDevice *vhci_device = USBEMU_VHCI_DEVICE (object);

  switch (prop_id) {
    case PROP_DEVID:
      g_value_set_uint (value, usbemu_vhci_device_get_devid (vhci_device));
      break;
    case PROP_PORT:
      g_value_set_uint (value, usbemu_vhci_device_get_port (vhci_device));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
device_class_attached (UsbemuDevice *device)
{
  UsbemuVhciDevice *vhci_device;
  UsbemuVhciDevicePrivate *priv;

  vhci_device = USBEMU_VHCI_DEVICE (device);
  priv = USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device);
  g_buffered_input_stream_fill_async ((GBufferedInputStream*) priv->istream,
                                      sizeof (struct usbip_header),
                                      G_PRIORITY_DEFAULT,
                                      priv->cancellable,
                                      (GAsyncReadyCallback) _on_istream_fill_callback,
                                      vhci_device);
}

static gboolean
_extract_attach_options_from_strv (gchar        **strv,
                                   guint         *port,
                                   UsbemuSpeeds  *speed)
{
  static const UsbemuSpeeds available_speeds[] = {
    USBEMU_SPEED_HIGH,
    USBEMU_SPEED_SUPER,
  };
  const gchar *str, *speed_str;
  gsize i;

  if (strv == NULL)
    return TRUE;

  while ((str = *strv++) != NULL) {
    if (g_ascii_strncasecmp (str, USBEMU_VHCI_DEVICE_PROP_PORT "=",
                             strlen (USBEMU_VHCI_DEVICE_PROP_PORT "=")) == 0) {
      *port = strtoul (strchr (str, '=') + 1, NULL, 0);
      continue;
    }

    if (g_ascii_strncasecmp (str, "speed=", 6) == 0) {
      str += 6;
      for (i = 0; i < G_N_ELEMENTS (available_speeds); i++) {
        speed_str = usbemu_speeds_get_string (available_speeds[i]);
        if (g_ascii_strcasecmp (str, speed_str) == 0) {
          *speed = available_speeds[i];
          break;
        }
      }

      if (i < G_N_ELEMENTS (available_speeds))
        continue;

      /* fallback for errors */
    }

    return FALSE;
  }

  return TRUE;
}

typedef struct _AttachData {
  guint port;
  UsbemuSpeeds speed;

#define ATTACH_DATA_SERVER_CONNECTION 0
#define ATTACH_DATA_CLIENT_CONNECTION 1
  GSocketConnection *connections[2];
} AttachData;

static void
_attach_data_free (AttachData *attach_data)
{
  if (attach_data->connections[ATTACH_DATA_SERVER_CONNECTION] != NULL)
    g_object_unref (attach_data->connections[ATTACH_DATA_SERVER_CONNECTION]);
  if (attach_data->connections[ATTACH_DATA_CLIENT_CONNECTION] != NULL)
    g_object_unref (attach_data->connections[ATTACH_DATA_CLIENT_CONNECTION]);

  g_slice_free(AttachData, attach_data);
}

static void
device_class_attach_async (UsbemuDevice  *device,
                           GTask         *task,
                           gchar        **options)
{
  guint port = USBEMU_VHCI_DEVICE_PORT_UNSPECIFIED;
  UsbemuSpeeds speed = USBEMU_SPEED_HIGH;
  gchar port_str[16];
  AttachData *attach_data;

  if (!_extract_attach_options_from_strv (options, &port, &speed)) {
    g_task_return_new_error (task, USBEMU_ERROR, USBEMU_ERROR_SYNTAX_ERROR,
                             "failed to parse attach options");
    return;
  }

  if (port == USBEMU_VHCI_DEVICE_PORT_UNSPECIFIED)
    snprintf (port_str, sizeof (port_str), "unspecified");
  else
    snprintf (port_str, sizeof (port_str), "%u", port);
  g_debug (LOG_COMPONENT "prepare to attach VHCI device 0x%08x to port %s, speed %s",
           usbemu_vhci_device_get_devid ((UsbemuVhciDevice*) device),
           port_str,
           usbemu_speeds_get_string (speed));

  attach_data = g_slice_new0 (AttachData);
  attach_data->port = port;
  attach_data->speed = speed;
  g_task_set_task_data (task, attach_data, (GDestroyNotify) _attach_data_free);

  _setup_connections_for_attach (task);
}

static void
device_class_detach_async (UsbemuDevice  *device,
                           GTask         *task,
                           gchar        **options G_GNUC_UNUSED)
{
  guint port;
  GError *error = NULL;

  if (!usbemu_device_get_attached (device)) {
    g_task_return_new_error (task, USBEMU_ERROR, USBEMU_ERROR_INVALID_STATE,
                             "device is not currently attached");
    return;
  }

  port = usbemu_vhci_device_get_port (USBEMU_VHCI_DEVICE (device));
  /* kernel should close the socket, and we'll handle disconnection there. */
  if (!usbemu_vhci_device_detach_port (port, &error))
    g_task_return_error (task, error);
  else
    g_task_return_boolean (task, TRUE);
}

static void
usbemu_vhci_device_class_init (UsbemuVhciDeviceClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  UsbemuDeviceClass *device_class = USBEMU_DEVICE_CLASS (klass);

  /* virtual methods */

  object_class->set_property = object_class_set_property;
  object_class->get_property = object_class_get_property;

  device_class->attached = device_class_attached;
  device_class->attach_async = device_class_attach_async;
  device_class->detach_async = device_class_detach_async;

  /* properties */

  /**
   * UsbemuVhciDevice:devid:
   */
  props[PROP_DEVID] =
    g_param_spec_uint (USBEMU_VHCI_DEVICE_PROP_DEVID,
                       "Device ID", "Device ID",
                       0, G_MAXUINT, 0,
                       G_PARAM_READWRITE | \
                         G_PARAM_CONSTRUCT);

  /**
   * UsbemuVhciDevice:port:
   *
   * Port number this VHCI device is attached to.
   */
  props[PROP_PORT] =
    g_param_spec_uint (USBEMU_VHCI_DEVICE_PROP_PORT,
                       "Port", "Port",
                       0, G_MAXUINT, USBEMU_VHCI_DEVICE_PORT_UNSPECIFIED,
                       G_PARAM_READABLE);

  g_object_class_install_properties (object_class, N_PROPERTIES, props);
}

static void
usbemu_vhci_device_init (UsbemuVhciDevice *vhci_device)
{
  UsbemuVhciDevicePrivate *priv = USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device);

  priv->devid = 0;
  priv->port = USBEMU_VHCI_DEVICE_PORT_UNSPECIFIED;

  priv->connection = NULL;
  priv->cancellable = NULL;
  priv->istream = NULL;
  priv->ostream = NULL;

  priv->rx_pending = NULL;
  priv->tx_queue = NULL;
}

/**
 * usbemu_vhci_device_new:
 *
 * Create a new #UsbemuVhciDevice instance.
 *
 * Returns: (transfer full) (type UsbemuVhciDevice): The constructed device
 *          object or %NULL if failed.
 */
UsbemuVhciDevice*
usbemu_vhci_device_new ()
{
  return g_object_new (USBEMU_TYPE_VHCI_DEVICE, NULL);
}

/**
 * usbemu_vhci_device_get_devid:
 * @vhci_device: (in): A #UsbemuVhciDevice object.
 *
 * Get VHCI device id.
 *
 * Returns: VHCI Device devid.
 */
guint32
usbemu_vhci_device_get_devid (UsbemuVhciDevice *vhci_device)
{
  g_return_val_if_fail (USBEMU_IS_VHCI_DEVICE (vhci_device), 0);

  return USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device)->devid;
}

/**
 * usbemu_vhci_device_set_devid:
 * @vhci_device: (in): A #UsbemuVhciDevice object.
 * @devid: unique device identifier.
 *
 * Set VHCI device id. devid is unique device identifier in a remote host in
 * usbip protocol. In USBEmu it's irrelevant because we have no physical device
 * to identify at all.
 */
void
usbemu_vhci_device_set_devid (UsbemuVhciDevice *vhci_device,
                              guint32           devid)
{
  g_return_if_fail (USBEMU_IS_VHCI_DEVICE (vhci_device));

  USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device)->devid = devid;
}

/**
 * usbemu_vhci_device_get_port:
 * @vhci_device: (in): a #UsbemuVhciDevice object.
 *
 * Get VHCI Device port.
 *
 * Returns: VHCI Device port.
 */
guint
usbemu_vhci_device_get_port (UsbemuVhciDevice *vhci_device)
{
  g_return_val_if_fail (USBEMU_IS_VHCI_DEVICE (vhci_device),
                        USBEMU_VHCI_DEVICE_PORT_UNSPECIFIED);

  return USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device)->port;
}

static void
_usbemu_vhci_device_set_port (UsbemuVhciDevice *vhci_device,
                              guint             port)
{
  USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device)->port = port;
  g_object_notify_by_pspec (G_OBJECT (vhci_device), props[PROP_PORT]);
}

static void
_cleanup_attached_state_from_error (UsbemuVhciDevice *vhci_device)
{
  UsbemuVhciDevicePrivate *priv;

  priv = USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device);
  g_cancellable_cancel (priv->cancellable);
  g_clear_object (&priv->cancellable);
  g_io_stream_close (G_IO_STREAM (priv->connection), NULL, NULL);
  g_clear_object (&priv->istream);
  g_clear_object (&priv->ostream);
  g_clear_object (&priv->connection);

  _usbemu_vhci_device_set_port (vhci_device,
                                USBEMU_VHCI_DEVICE_PORT_UNSPECIFIED);
  _usbemu_device_set_attached ((UsbemuDevice*) vhci_device,
                               FALSE, USBEMU_SPEED_UNKNOWN);
}

static void
_enqueue_tx_submit (UsbemuVhciDevicePrivate *priv,
                    guint32                  seqnum,
                    UsbemuUrb               *urb)
{
  g_debug (LOG_COMPONENT "tx enqueue submit: %u", seqnum);
}

static void
_enqueue_tx_unlink (UsbemuVhciDevicePrivate *priv,
                    guint32                  seqnum,
                    guint32                  status)
{
  g_debug (LOG_COMPONENT "tx enqueue unlink: %u", seqnum);
#if 0
  struct usbip_header *header;

  header = g_slice_new0 (struct usbip_header);
  header->base.seqnum = seqnum;
  header->u.ret_unlink.status = status;
  priv->tx_queue = g_list_append (priv->tx_queue, header);
#endif
}

typedef struct _PendingUrbData {
  guint32 seqnum;
  guint32 unlink_seqnum;
  GCancellable *cancellable;
} PendingUrbData;

static void
_on_submit_urb_callback (UsbemuVhciDevice *vhci_device,
                         UsbemuUrb        *urb,
                         GAsyncResult     *result,
                         PendingUrbData   *pending)
{
  UsbemuVhciDevicePrivate *priv;
  GError *error = NULL;
  GList *list;

  priv = USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device);
  list = g_list_find (priv->rx_pending, pending);
  if (list != NULL) {
    g_object_unref (pending->cancellable);
    priv->rx_pending = g_list_delete_link (priv->rx_pending, list);
  }

  if (!usbemu_device_submit_urb_finish ((UsbemuDevice*) vhci_device,
                                        result, &error)) {
    if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
      _enqueue_tx_unlink (priv, pending->unlink_seqnum, -ECONNRESET);
    else
      _enqueue_tx_submit (priv, pending->seqnum, urb);
  } else
    _enqueue_tx_submit (priv, pending->seqnum, urb);

  g_slice_free (PendingUrbData, pending);
}

static void
_recv_cmd_submit (UsbemuVhciDevice    *vhci_device,
                  struct usbip_header *header)
{
  UsbemuVhciDevicePrivate *priv;
  UsbemuUrb *urb;
  UsbemuIsoPacketDescriptor *descriptor;
  struct usbip_iso_packet_descriptor iso_frame_desc;
  gsize i;
  PendingUrbData *pending;

  priv = USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device);

  urb = usbemu_urb_new ();
  _usbemu_usbip_cmd_submit_header_to_urb (&header->u.cmd_submit, urb);

  if ((header->base.direction == USBIP_DIR_OUT)
      && (urb->transfer_buffer_length != 0)) {
    g_input_stream_read (priv->istream,
                         urb->transfer_buffer, urb->transfer_buffer_length,
                         NULL, NULL);
  }

  if (/* FIXME: type iso */urb->number_of_packets != 0) {
    descriptor = urb->iso_frame_desc;
    for (i = 0; i < urb->number_of_packets; ++i, ++descriptor) {
      g_input_stream_read (priv->istream,
                           &iso_frame_desc, sizeof (iso_frame_desc),
                           NULL, NULL);
      _usbemu_usbip_iso_packet_correct_endian (&iso_frame_desc,
                                               USBEMU_ENDPOINT_DIRECTION_OUT);
      _usbemu_usbip_iso_packet_to_urb (&iso_frame_desc, descriptor);
    }
  }

  pending = g_slice_new0 (PendingUrbData);
  pending->seqnum = header->base.seqnum;
  pending->cancellable = g_cancellable_new ();
  priv->rx_pending = g_list_append (priv->rx_pending, pending);

  usbemu_device_submit_urb_async ((UsbemuDevice*) vhci_device,
                                  urb, pending->cancellable,
                                  (UsbemuSubmitUrbReadyCallback) _on_submit_urb_callback,
                                  pending);

  usbemu_urb_unref (urb);
}

static void
_recv_cmd_unlink (UsbemuVhciDevice    *vhci_device,
                  struct usbip_header *header)
{
  UsbemuVhciDevicePrivate *priv;
  GList *list;
  PendingUrbData *pending;

  priv = USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device);

  for (list = priv->rx_pending; list != NULL; list = list->next) {
    pending = list->data;
    if (pending->seqnum != header->u.cmd_unlink.seqnum)
      continue;

    pending->unlink_seqnum = header->base.seqnum;
    g_cancellable_cancel (pending->cancellable);
    return;
  }

  /* not found. */
  _enqueue_tx_unlink (priv, header->base.seqnum, 0);
}

static void
_handle_istream_packet_available (UsbemuVhciDevice    *vhci_device,
                                  struct usbip_header *header)
{
  _usbemu_usbip_header_dump (header);

  switch (header->base.command) {
    case USBIP_CMD_SUBMIT:
      _recv_cmd_submit (vhci_device, header);
      break;
    case USBIP_CMD_UNLINK:
      _recv_cmd_unlink (vhci_device, header);
      break;
    default:
      g_assert_not_reached ();
      break;
  }
}

static void
_handle_istream_header_available (UsbemuVhciDevice     *vhci_device,
                                  GBufferedInputStream *bistream)
{
  UsbemuVhciDevicePrivate *priv;
  struct usbip_header header;
  struct usbip_header_cmd_submit *cmd_submit;
  gsize total_len, available, size;

  total_len = sizeof (header);

  g_buffered_input_stream_peek (bistream, &header, 0, sizeof (header));
  _usbemu_usbip_header_correct_endian (&header, USBEMU_ENDPOINT_DIRECTION_OUT);

  switch (header.base.command) {
    case USBIP_CMD_SUBMIT:
      cmd_submit = &header.u.cmd_submit;
      if (header.base.direction == USBIP_DIR_OUT)
        total_len += cmd_submit->transfer_buffer_length;
      /* FIXME: type iso */total_len +=
          cmd_submit->number_of_packets * sizeof (struct usbip_iso_packet_descriptor);
      break;
    case USBIP_CMD_UNLINK:
      /* no extra payloads. */
      break;
    default:
      g_warning (LOG_COMPONENT "unknown command %u", header.base.command);
      _cleanup_attached_state_from_error (vhci_device);
      break;
  }

  available = g_buffered_input_stream_get_available (bistream);
  if (available < total_len) {
    size = g_buffered_input_stream_get_buffer_size (bistream);
    if (size < total_len)
      g_buffered_input_stream_set_buffer_size (bistream, total_len);

    priv = USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device);
    g_buffered_input_stream_fill_async (bistream,
                                        total_len - available,
                                        G_PRIORITY_DEFAULT,
                                        priv->cancellable,
                                        (GAsyncReadyCallback) _on_istream_fill_callback,
                                        vhci_device);
    return;
  }

  g_input_stream_skip ((GInputStream*) bistream, sizeof (header), NULL, NULL);

  _handle_istream_packet_available (vhci_device, &header);
}

static void
_on_istream_fill_callback (GBufferedInputStream *bistream,
                           GAsyncResult         *result,
                           UsbemuVhciDevice     *vhci_device)
{
  UsbemuVhciDevicePrivate *priv;
  gssize len;
  GError *error = NULL;

  len = g_buffered_input_stream_fill_finish (bistream, result, &error);
  if (len == -1) {
    g_debug (LOG_COMPONENT "failed to read from input stream. code %d: %s",
             error->code, error->message);

    /* if not cancelled, do further error handling. */
    if (!g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
      _cleanup_attached_state_from_error (vhci_device);

    g_error_free (error);
    return;
  }

  if (len == 0) {
    g_debug (LOG_COMPONENT "input stream EOF.");
    _cleanup_attached_state_from_error (vhci_device);
    return;
  }

  len = g_buffered_input_stream_get_available (bistream);
  if (len < sizeof (struct usbip_header)) {
    /* read again */
    priv = USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device);
    g_buffered_input_stream_fill_async (bistream,
                                        sizeof (struct usbip_header) - len,
                                        G_PRIORITY_DEFAULT,
                                        priv->cancellable,
                                        (GAsyncReadyCallback) _on_istream_fill_callback,
                                        vhci_device);
    return;
  }

  _handle_istream_header_available (vhci_device, bistream);
}

static gchar*
_socket_connection_local_address_to_string (GSocketConnection *connection)
{
  GSocketAddress *addr;
  gchar *str = NULL;

  addr = g_socket_connection_get_local_address (connection, NULL);
  if (addr != NULL) {
    str = g_socket_connectable_to_string (G_SOCKET_CONNECTABLE (addr));
    g_object_unref (addr);
  }

  return str;
}

static void
_dump_server_client_addr (GSocketConnection *server_conn,
                          GSocketConnection *client_conn)
{
  gchar *server_addr, *client_addr;

  server_addr = _socket_connection_local_address_to_string (server_conn);
  client_addr = _socket_connection_local_address_to_string (client_conn);
  g_info (LOG_COMPONENT "server: %s, client: %s", server_addr, client_addr);

  if (server_addr != NULL)
    g_free (server_addr);
  if (client_addr != NULL)
    g_free (client_addr);
}

static void
_on_connections_ready (GTask             *task,
                       guint              port,
                       UsbemuSpeeds       speed,
                       GSocketConnection *server_conn,
                       GSocketConnection *client_conn)
{
  UsbemuVhciDevice *vhci_device;
  UsbemuVhciDevicePrivate *priv;
  enum usb_device_speed kernel_speed;
  gint fd;
  gboolean success = TRUE;
  GError *error = NULL;

  _dump_server_client_addr (server_conn, client_conn);

  vhci_device = USBEMU_VHCI_DEVICE (g_task_get_source_object (task));
  priv = USBEMU_VHCI_DEVICE_GET_PRIVATE (vhci_device);
  if (usbemu_device_get_attached ((UsbemuDevice*) vhci_device)) {
    success = FALSE;
    error = g_error_new (USBEMU_ERROR, USBEMU_ERROR_INVALID_STATE,
                         "device is alreaddy attached at port %u",
                         usbemu_vhci_device_get_port (vhci_device));
  } else {
    fd = g_socket_get_fd (g_socket_connection_get_socket (client_conn));
    kernel_speed =
          (speed == USBEMU_SPEED_SUPER) ? USB_SPEED_SUPER : USB_SPEED_HIGH;
    success = _vhci_attach_device (&port, fd, priv->devid, kernel_speed, &error);
  }

  if (!success) {
    /* Will be freed in #_attach_data_free(). Just close here. */
    g_io_stream_close (G_IO_STREAM (server_conn), NULL, NULL);
    g_io_stream_close (G_IO_STREAM (client_conn), NULL, NULL);

    g_task_return_error (task, error);
    return;
  }

  /* Still close client connection for it's now managed by kernel. */
  g_io_stream_close (G_IO_STREAM (client_conn), NULL, NULL);

  priv->connection = g_object_ref (server_conn);
  priv->istream = g_buffered_input_stream_new (
        g_io_stream_get_input_stream (G_IO_STREAM (server_conn)));
  priv->ostream = g_buffered_output_stream_new (
        g_io_stream_get_output_stream (G_IO_STREAM (server_conn)));
  priv->cancellable = g_cancellable_new ();

  _usbemu_vhci_device_set_port (vhci_device, port);
  _usbemu_device_set_attached ((UsbemuDevice*) vhci_device, TRUE, speed);

  g_message (LOG_COMPONENT "attached VHCI device 0x%08x to port %u speed %s",
             usbemu_vhci_device_get_devid (vhci_device), port,
             usbemu_speeds_get_string (speed));

  g_task_return_boolean (task, TRUE);
}

static void
_on_new_connection (GTask             *task,
                    GError            *error,
                    GSocketConnection *connection,
                    gint               index)
{
  AttachData *attach_data;

  if (connection == NULL) {
    if (!g_task_had_error (task))
      g_task_return_error (task, error);
    else
      g_error_free (error);

    g_object_unref (task);
    return;
  }

  if (g_task_had_error (task)) {
    g_object_unref (connection);
    g_object_unref (task);
    return;
  }

  attach_data = g_task_get_task_data (task);
  attach_data->connections[index] = connection;
  if (attach_data->connections[1 - index] != NULL) {
    /* both sides are completed. */
    _on_connections_ready (task, attach_data->port, attach_data->speed,
                           attach_data->connections[ATTACH_DATA_SERVER_CONNECTION],
                           attach_data->connections[ATTACH_DATA_CLIENT_CONNECTION]);
  }

  g_object_unref (task);
}

static void
_on_client_connect_callback (GSocketClient *client,
                             GAsyncResult  *result,
                             GTask         *task)
{
  GSocketConnection *client_conn;
  GError *error = NULL;

  client_conn = g_socket_client_connect_finish (client, result, &error);
  g_object_unref (client);

  _on_new_connection (task, error, client_conn, ATTACH_DATA_CLIENT_CONNECTION);
}

static void
_on_server_accept_callback (GSocketListener *listener,
                            GAsyncResult    *result,
                            GTask           *task)
{
  GSocketConnection *server_conn;
  GError *error = NULL;

  server_conn = g_socket_listener_accept_finish (listener, result,
                                                 NULL, &error);
  g_socket_listener_close (listener);
  g_object_unref (listener);

  _on_new_connection (task, error, server_conn, ATTACH_DATA_SERVER_CONNECTION);
}

static void
_setup_connections_for_attach (GTask *task)
{
  GInetAddress *loopback_addr;
  GSocketAddress *loopback_sock_addr;
  GSocketAddress *effective_sock_addr = NULL;
  GSocketListener *listener;
  GSocketClient *client;
  GError *error = NULL;
  gboolean ret;

  loopback_addr = g_inet_address_new_loopback (G_SOCKET_FAMILY_IPV4);
  loopback_sock_addr = g_inet_socket_address_new (loopback_addr, 0);
  g_object_unref (loopback_addr);

  listener = g_socket_listener_new ();
  ret = g_socket_listener_add_address (listener,
                                       loopback_sock_addr,
                                       G_SOCKET_TYPE_STREAM,
                                       G_SOCKET_PROTOCOL_TCP,
                                       NULL, &effective_sock_addr,
                                       &error);
  g_object_unref (loopback_sock_addr);
  if (!ret) {
    g_socket_listener_close (listener);
    g_object_unref (listener);

    g_task_return_error (task, error);
    g_object_unref (task);
    return;
  }

  g_socket_listener_accept_async (listener,
                                  g_task_get_cancellable (task),
                                  (GAsyncReadyCallback) _on_server_accept_callback,
                                  g_object_ref (task));

  client = g_socket_client_new ();
  g_socket_client_connect_async (client,
                                 G_SOCKET_CONNECTABLE (effective_sock_addr),
                                 g_task_get_cancellable (task),
                                 (GAsyncReadyCallback) _on_client_connect_callback,
                                 g_object_ref (task));

  g_object_unref (effective_sock_addr);
}

/**
 * usbemu_vhci_device_detach_port:
 * @port: an attached VHCI port to deattach.
 * @error: (out): #GError for error reporting, or %NULL to ignore.
 *
 * Detach a currently unmanaged, connected VHCI port.
 *
 * Returns: the deattach result. #FALSE on error.
 */
gboolean
usbemu_vhci_device_detach_port (guint    port,
                                GError **error)
{
  return _vhci_detach_device (port, error);
}
