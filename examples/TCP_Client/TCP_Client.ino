

#include "WisLTEBG96TCPIP.h"
#include "SoftwareSerial.h"

#define DSerial Serial1
#define ATSerial Serial
// #define AT_TX_PIN  11
// #define AT_RX_PIN  10
// SoftwareSerial  DSerial(AT_RX_PIN, AT_TX_PIN);

const char APN[] = "hologram";
const char tcp_ip[] = "mbed.org";
const int tcp_port = 80;
const char send_data[] = "GET /media/uploads/mbed_official/hello.txt HTTP/1.0\r\n\r\n";
unsigned int comm_pdp_index = 2;  // The range is 1 ~ 16
unsigned int comm_socket_index = 2;  // The range is 0 ~ 11
Socket_Type_t socket = TCP_CLIENT;

WisLTEBG96TCPIP WisLTE(ATSerial, DSerial);

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

    while (!WisLTE.OpenSocketService(comm_pdp_index, comm_socket_index, socket, tcp_ip, tcp_port, 0, BUFFER_MODE)){
        DSerial.println("\r\nOpen Socket Service Fail!");
    }
    DSerial.println("\r\nOpen Socket Service Success!");

    if(WisLTE.SocketSendData(comm_socket_index, socket, send_data, "", "")){
        DSerial.println("\r\nSocket Send Data Success!");
    }

}

void loop(){
    char m_event[16];
    unsigned int index;
    char recv_data[128];
    Socket_Event_t ret = WisLTE.WaitCheckSocketEvent(m_event, 2);
    switch(ret)
    {
        case SOCKET_CLOSE_EVENT:
            index = atoi(m_event);
            if(WisLTE.CloseSocketService(index)){
                DSerial.println("\r\nClose Socket Success!");
            }
            break;
        case SOCKET_RECV_DATA_EVENT:
            index = atoi(m_event);
            if (WisLTE.SocketRecvData(index, 128, socket, recv_data)){
                DSerial.println("\r\nSocket Recv Data Success!");
                DSerial.println("");
                DSerial.println(recv_data);
                DSerial.println("");
            }
            break;
        case SOCKET_PDP_DEACTIVATION_EVENT:
            index = atoi(m_event);
            if(WisLTE.DeactivateDevAPN(index)){
                DSerial.println("\r\nPlease reconfigure APN!");
            }
            break;
        default:
            break;
    }
}
