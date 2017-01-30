/*
 * Copyright (c) 2017, Weptech elektronik GmbH Germany
 * http://www.weptech.de
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 *      mqtt-client.c
 */

#include "project-conf.h"
#include "contiki.h"

#include "mqtt-client.h"
#include "mqtt.h"

#include "rpl.h"

#include <string.h>

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&mqtt_client_process);
/*---------------------------------------------------------------------------*/
#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
static struct mqtt_connection conn;
static mqtt_configuration conf = { MQTT_BROKER_IP,
                  MQTT_BROKER_PORT,
                  MQTT_PUBLISH_INTERVAL,
                  MQTT_CLIENT_DEFAULT_ORG_ID,
                  MQTT_CLIENT_DEFAULT_TYPE_ID };

static mqtt_client_message msg;
static char client_id[MQTT_CLIENT_ID_LEN];
/*---------------------------------------------------------------------------*/

void
mqtt_event_handler(struct mqtt_connection *m, mqtt_event_t event, void *data){
  switch (event) {
    case MQTT_EVENT_CONNECTED:
      process_poll(&mqtt_client_process);
      break;
    case MQTT_EVENT_DISCONNECTED:
      process_poll(&mqtt_client_process);
      break;

//    case MQTT_EVENT_SUBACK:
//      break;
//    case MQTT_EVENT_UNSUBACK:
//      break;
//    case MQTT_EVENT_PUBLISH:
//      break;
//    case MQTT_EVENT_PUBACK:
//      break;
//
//      /* Errors */
//    case MQTT_EVENT_ERROR:
//      break;
//    case MQTT_EVENT_PROTOCOL_ERROR:
//      break;
//    case MQTT_EVENT_CONNECTION_REFUSED_ERROR:
//      break;
//    case MQTT_EVENT_DNS_ERROR:
//      break;
//    case MQTT_EVENT_NOT_IMPLEMENTED_ERROR:
//      break;
    default:
      PRINTF("APP - Application got a unhandled MQTT event: %i\n", event);
      break;

  }
}

/*---------------------------------------------------------------------------*/
mqtt_status_t
client_mqtt_register(){
  mqtt_status_t ret;

  snprintf(client_id, MQTT_CLIENT_ID_LEN, "d:%s:%s:%02x%02x%02x%02x%02x%02x",
           conf.org_id, conf.type_id,
           linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1],
           linkaddr_node_addr.u8[2], linkaddr_node_addr.u8[5],
           linkaddr_node_addr.u8[6], linkaddr_node_addr.u8[7]);

  PRINTF("APP attempting to register.\n");
  ret = mqtt_register(&conn, &mqtt_client_process, client_id,
      mqtt_event_handler,
      MQTT_CLIENT_MAX_SEGMENT_SIZE);

  /*
   * Autoreconnect can bring the connection into a state where the MQTT app
   * compulsively opens very many sockets.
   */
  conn.auto_reconnect = 0;
  conn.state = MQTT_CONN_STATE_NOT_CONNECTED;

  return ret;
}
/*---------------------------------------------------------------------------*/
mqtt_status_t
client_mqtt_connect(){
  mqtt_status_t ret = MQTT_STATUS_OK;
  if (conn.state == MQTT_CONN_STATE_NOT_CONNECTED) {
    PRINTF("APP attempting to connect.\n");
    ret = mqtt_connect(&conn,
        conf.host, conf.port, conf.publish_interval * 3);
  }
  return ret;
}
/*---------------------------------------------------------------------------*/
static void
construct_message(mqtt_client_message* msg){
  static int seq_no = 0;

  seq_no++;

  msg->id = seq_no;

  snprintf(msg->topic, MQTT_CLIENT_TOPIC_LEN, "iot-2/evt/status/fmt/json");

  int size = snprintf((char *)(msg->payload), MQTT_CLIENT_PAYLOAD_LEN,
      "{\"d\": {"
      "\"board_string\": \"%s\","
      "\"seq_no\": %u,"
      "\"uptime\": %lu"
      "}}",
      BOARD_STRING,
      seq_no,
      clock_seconds()
      );
  if(size >= MQTT_CLIENT_PAYLOAD_LEN){
    PRINTF("MQTT_CLIENT_PAYLOAD_LEN is too small!");
  }

  msg->size = size;
}
/*---------------------------------------------------------------------------*/
mqtt_status_t
client_mqtt_publish(){

  mqtt_status_t ret = MQTT_STATUS_OK;

  /*
   * Quick successive publishes can lock up the MQTT process, so we check
   * whether the previous message is fully out the door first.
   */
  if (conn.state == MQTT_CONN_STATE_CONNECTED_TO_BROKER &&
      !(conn.out_queue_full || !conn.out_buffer_sent)) {
    construct_message(&msg);
    ret = mqtt_publish(&conn, &msg.id, msg.topic, msg.payload, msg.size,
        MQTT_QOS_LEVEL_0, MQTT_RETAIN_OFF);
  }

  return ret;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mqtt_client_process, ev, data)
{
  static struct etimer publish_periodic_timer;

  PROCESS_BEGIN();

  PRINTF("APP Process waiting.\n");

  /* Wait until we connect to the RPL DAG. */
  while (rpl_get_any_dag() == (rpl_dag_t *)NULL){
    etimer_set(&publish_periodic_timer, CLOCK_SECOND);
    PROCESS_YIELD();
  }
  etimer_stop(&publish_periodic_timer);

  PRINTF("APP Process started.\n");
  client_mqtt_register();
  client_mqtt_connect();

  while(1) {

    PROCESS_YIELD();

    /* MQTT event handler got an event from the MQTT app. */
    if(ev == PROCESS_EVENT_POLL){
      if (conn.state == MQTT_CONN_STATE_CONNECTED_TO_BROKER){
        etimer_set(&publish_periodic_timer, conf.publish_interval *
        CLOCK_SECOND);
      } else {
        /* If disconnected, try reconnecting. */
        etimer_stop(&publish_periodic_timer);
        client_mqtt_connect();
      }
    }

    if (ev == PROCESS_EVENT_TIMER && data == &publish_periodic_timer) {
      client_mqtt_publish();
      etimer_restart(&publish_periodic_timer);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
