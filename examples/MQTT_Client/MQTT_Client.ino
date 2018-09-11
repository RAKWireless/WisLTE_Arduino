
#include "WisLTEBG96MQTT.h"
#include "SoftwareSerial.h"

#define DSerial Serial1
#define ATSerial Serial
// #define AT_TX_PIN  11
// #define AT_RX_PIN  10
// SoftwareSerial  DSerial(AT_RX_PIN, AT_TX_PIN);

const char APN[] = "hologram";
const char mqtt_server[] = "iot-as-mqtt.cn-shanghai.aliyuncs.com";
unsigned int mqtt_port = 1883;
const char mqtt_client_id[] = "mqtt_test";
unsigned int comm_pdp_index = 1;    // The range is 1 ~ 16
unsigned int comm_mqtt_index = 1; // The range is 0 ~ 5
Mqtt_Version_t version = MQTT_V3;
Mqtt_Qos_t mqtt_qos = AT_MOST_ONCE;
const char produck_key[] = "a1mQBMY2HI9";
const char device_name[] = "D896E0FF00012940";
const char device_secret[] = "GVQqGv6Y3hd3q9n8EXLHW95Bpgs8HG3W";

WisLTEBG96MQTT WisLTE(ATSerial, DSerial);

void setup(){
    DSerial.begin(115200);
    while (DSerial.read() >= 0);
    DSerial.println("This is the WisLTE Debug Serial!");
    delay(1000);
    ATSerial.begin(115200);
    while (ATSerial.read() >= 0);
    delay(1000);
    while(!WisLTE.InitModule());
    
    WisLTE.SetDevCommandEcho(false);

    char inf[64];
    if(WisLTE.GetDevInformation(inf)){
        DSerial.println(inf);
    }

    char apn_error[64];
    while (!WisLTE.InitAPN(comm_pdp_index, APN, "", "", apn_error)){
        DSerial.println(apn_error);
    }
    DSerial.println(apn_error);

    while(!WisLTE.SetMQTTConfigureParameters(comm_mqtt_index, comm_pdp_index, version, 150, SERVER_STORE_SUBSCRIPTIONS)){
        DSerial.println("\r\nConfig the MQTT Parameter Fail!");
        int e_code;
        if (WisLTE.returnErrorCode(e_code)){
            DSerial.print("\r\nERROR CODE: ");
            DSerial.println(e_code);
            DSerial.println("Please check the documentation for error details.");
            while (1);
        }
    }
    DSerial.println("\r\nConfig the MQTT Parameter Success!");

    while(!WisLTE.SetMQTTAlibabaCloudDeviceInformation(comm_mqtt_index, produck_key, device_name, device_secret)){
        DSerial.println("\r\nSet the Alibaba Cloud MQTT Fail!");
        int e_code;
        if (WisLTE.returnErrorCode(e_code)){
            DSerial.print("\r\nERROR CODE: ");
            DSerial.println(e_code);
            DSerial.println("Please check the documentation for error details.");
            while (1);
        }
    }
    DSerial.println("\r\nSet the Alibaba Cloud MQTT Success!");

    while (WisLTE.OpenMQTTNetwork(comm_mqtt_index, mqtt_server, mqtt_port) != 0){
        DSerial.println("\r\nSet the MQTT Service Address Fail!");
        int e_code;
        if (WisLTE.returnErrorCode(e_code)){
            DSerial.print("\r\nERROR CODE: ");
            DSerial.println(e_code);
            DSerial.println("Please check the documentation for error details.");
            while (1);
        }
    }
    DSerial.println("\r\nSet the MQTT Service Address Success!");

    while (WisLTE.CreateMQTTClient(comm_mqtt_index, mqtt_client_id, "", "") != 0){
        DSerial.println("\r\nCreate a MQTT Client Fail!");
        int e_code;
        if (WisLTE.returnErrorCode(e_code)){
            DSerial.print("\r\nERROR CODE: ");
            DSerial.println(e_code);
            DSerial.println("Please check the documentation for error details.");
            while (1);
        }
    }
    DSerial.println("\r\nCreate a MQTT Client Success!");

    while (WisLTE.MQTTSubscribeTopic(comm_mqtt_index, 1, "/a1mQBMY2HI9/D896E0FF00012940/update", mqtt_qos) != 0){
        DSerial.println("\r\nMQTT Subscribe Topic Fail!");
        int e_code;
        if (WisLTE.returnErrorCode(e_code)){
            DSerial.print("\r\nERROR CODE: ");
            DSerial.println(e_code);
            DSerial.println("Please check the documentation for error details.");
            while (1);
        }
    }
    DSerial.println("\r\nMQTT Subscribe Topic Success!");
}

void loop(){
    char mqtt_recv[128];
    char *sta_buf;
    Mqtt_URC_Event_t ret = WisLTE.WaitCheckMQTTURCEvent(mqtt_recv, 2);
    switch (ret)
    {
        case MQTT_RECV_DATA_EVENT:
            *sta_buf = strstr(mqtt_recv, "\",\"");
            DSerial.println("\r\nThe MQTT Recv Data");
            DSerial.println(sta_buf + 3);
            break;
        case MQTT_STATUS_EVENT:
            *sta_buf = strchr(mqtt_recv, ',');
            if (atoi(sta_buf + 1) == 1){
                if (WisLTE.CloseMQTTClient(comm_mqtt_index)){
                    DSerial.println("\r\nClose the MQTT Client Success!");
                }
            } else {
                DSerial.print("\r\nStatus cade is :");
                DSerial.println(atoi(sta_buf + 1));
                DSerial.println("Please check the documentation for error details.");
            }
            break;
    }
    // if (WisLTE.MQTTPublishMessages(comm_mqtt_index, 1, mqtt_qos, "/a1mQBMY2HI9/D896E0FF00012940/get", true, "1234") == 0){
    //     DSerial.println("\r\nMQTT Publish Messages Success!");
    //     delay(10000);
    // }
}