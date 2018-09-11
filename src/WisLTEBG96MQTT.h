/*
 * A library for RAK WisLTE BG96 Development board
 * This file is about the BG96 HTTP function
 * 
 * Copyright (c) 2018 RAKWireless technology inc.
 * @Author       : Chace
 * @Create time  : 09/07/2018
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _WISLTEBG96MQTT_H_
#define _WISLTEBG96MQTT_H_

#include "WisLTEBG96SSL.h"

typedef enum mqtt_version{
    MQTT_V4 = 4,
    MQTT_V3 = 3,
}Mqtt_Version_t;

typedef enum mqtt_qos{
    AT_MOST_ONCE = 0,
    AT_LEAST_ONCE = 1,
    EXACTLY_ONCE = 2,
}Mqtt_Qos_t;

typedef enum mqtt_session_type{
    SERVER_STORE_SUBSCRIPTIONS = 0,
    SERVER_DISCARD_INFORMATION = 1,
}Mqtt_Session_Type_t;

typedef enum mqtt_network_result_event{
    MQTT_NETWORK_OPEN_FAIL = -1,
    MQTT_NETWORK_OPEN_SUCCESS = 0,
    MQTT_NETWORK_WRONG_PARAMETER = 1,
    MQTT_NETWORK_INDEX_OCCUPIED = 2,
    MQTT_NETWORK_ACTIVATE_PDP_FAIL = 3,
    MQTT_NETWORK_PARSE_DOMAIN_FAIL = 4,
    MQTT_NETWORK_DISCONNECTION_ERROR = 5,
}Mqtt_Network_Result_t;

typedef enum mqtt_client_result_status{
    PACKET_SEND_SUCCESS_AND_RECV_ACK = 0,
    PACKET_RETRANSMISSION = 1,
    PACKET_SEND_FAIL = 2,
}Mqtt_Client_Result_Status_t;

typedef enum mqtt_urc_event{
    MQTT_STATUS_EVENT = 0,
    MQTT_RECV_DATA_EVENT = 1,
}Mqtt_URC_Event_t;

class WisLTEBG96MQTT : public WisLTEBG96SSL
{
  public:
    WisLTEBG96MQTT();

    ~WisLTEBG96MQTT();

    WisLTEBG96MQTT(Stream &atserial, Stream &dserial);

    bool SetMQTTConfigureParameters(unsigned int mqtt_index, unsigned int pdp_index, Mqtt_Version_t version, unsigned int keep_alive_time, Mqtt_Session_Type_t type);

    bool SetMQTTEnableSSL(unsigned int mqtt_index, unsigned int ssl_index, bool enable);

    bool SetMQTTMessageTimeout(unsigned int mqtt_index, unsigned int pkt_timeout, unsigned int retry_num, bool timeout_notice);

    bool SetMQTTAlibabaCloudDeviceInformation(unsigned int mqtt_index, char *product_key, char *device_name, char *device_secret);

    Mqtt_Network_Result_t OpenMQTTNetwork(unsigned int mqtt_index, char *host_name, unsigned int port);

    bool CloseMQTTNetwork(unsigned int mqtt_index);

    Mqtt_Client_Result_Status_t CreateMQTTClient(unsigned int mqtt_index, char *client_id, char *username, char *password);

    bool CloseMQTTClient(unsigned int mqtt_index);

    Mqtt_Client_Result_Status_t MQTTSubscribeTopic(unsigned int mqtt_index, unsigned int msg_id, char *topic, Mqtt_Qos_t qos);

    Mqtt_Client_Result_Status_t MQTTUnsubscribeTopic(unsigned int mqtt_index, unsigned int msg_id, char *topic);

    Mqtt_Client_Result_Status_t MQTTPublishMessages(unsigned int mqtt_index, unsigned int msg_id, Mqtt_Qos_t qos, char *topic, bool retain, char *publish_data);

    Mqtt_URC_Event_t WaitCheckMQTTURCEvent(char *event, unsigned int timeout);

  private:

};

#endif