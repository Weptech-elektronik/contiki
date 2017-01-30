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
/*---------------------------------------------------------------------------*/
#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_
/*---------------------------------------------------------------------------*/
/* Basic RPL configuration */
#define IEEE802154_CONF_PANID                   0xABCD
#define RF_CORE_CONF_CHANNEL                    25
/*---------------------------------------------------------------------------*/
/* Configuration of the MQTT broker */
//#define MQTT_BROKER_IP                          "0000:0000:0000:0000:0000:ffff:c0a8:6401" //localhost
#define MQTT_BROKER_IP                          "0000:0000:0000:0000:0000:ffff:b8ac:7cbd" // IBM
//#define MQTT_BROKER_IP                          "0000:0000:0000:0000:0000:ffff:ac11:0001" //docker
//#define MQTT_BROKER_IP                          "fd00:0000:0000:0000:0000:0000:0000:0001" //Slip
//#define MQTT_BROKER_PORT                        50000
#define MQTT_BROKER_PORT                        1883
/*---------------------------------------------------------------------------*/
/* Configuration of the MQTT app. */
#define MQTT_CLIENT_MAX_SEGMENT_SIZE            128
/*---------------------------------------------------------------------------*/
/* Configuration of the MQTT messages, buffer sizes, etc. */
#define MQTT_PUBLISH_INTERVAL                   10
#define MQTT_CLIENT_TOPIC_LEN                   26
#define MQTT_CLIENT_PAYLOAD_LEN                 127
#define MQTT_CLIENT_CONFIG_ORG_ID_LEN           11
#define MQTT_CLIENT_DEFAULT_ORG_ID              "quickstart"
#define MQTT_CLIENT_CONFIG_TYPE_ID_LEN          5
#define MQTT_CLIENT_DEFAULT_TYPE_ID             "coua"
/*
 * The format is "d:<org_id>:<type_id>:<device_id>". Counting the 'd', three
 * colons, the twelve character device ID and the terminating \0 gives 17.
 */
#define MQTT_CLIENT_ID_LEN                      MQTT_CLIENT_CONFIG_ORG_ID_LEN + MQTT_CLIENT_CONFIG_TYPE_ID_LEN + 17



#define UIP_CONF_BUFFER_SIZE                    900
#define NBR_TABLE_CONF_MAX_NEIGHBORS            5
#define UIP_CONF_MAX_ROUTES                     5
#define UIP_CONF_TCP_MSS                        128
#define UIP_CONF_ND6_RETRANS_TIMER  10000
/*---------------------------------------------------------------------------*/
#endif /* PROJECT_CONF_H_ */
/*---------------------------------------------------------------------------*/
