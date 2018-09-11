

#include "WisLTEBG96TCPIP.h"
#include "SoftwareSerial.h"

#define DSerial Serial1
#define ATSerial Serial
// #define AT_TX_PIN  11
// #define AT_RX_PIN  10
// SoftwareSerial  DSerial(AT_RX_PIN, AT_TX_PIN);

const char APN[] = "hologram";
const char tcps_ip[] = "127.0.0.1";
unsigned int tcp_port = 3030;
unsigned int comm_pdp_index = 3;    // The range is 1 ~ 16
unsigned int comm_socket_index = 3; // The range is 0 ~ 11
Socket_Type_t socket = TCP_SEVER;

WisLTEBG96TCPIP WisLTE(ATSerial, DSerial);

void setup()
{
    DSerial.begin(115200);
    while (DSerial.read() >= 0);
    DSerial.println("This is the WisLTE Debug Serial!");
    delay(100);
    ATSerial.begin(115200);
    while (ATSerial.read() >= 0);
    delay(100);
    while(!WisLTE.InitModule());
    
    WisLTE.SetDevCommandEcho(false);

    char inf[64];
    if (WisLTE.GetDevInformation(inf)){
        DSerial.println(inf);
    }

    char apn_error[64];
    while (!WisLTE.InitAPN(comm_pdp_index, APN, "", "", apn_error)){
        DSerial.println(apn_error);
    }
    DSerial.println(apn_error);

    while (!WisLTE.OpenSocketService(comm_pdp_index, comm_socket_index, socket, tcps_ip, 0, tcp_port, BUFFER_MODE)){
        DSerial.println("\r\nOpen Socket Service Fail!");
    }
    DSerial.println("\r\nOpen Socket Service Success!");
}

void loop()
{
    char m_event[32];
    char recv_data[128];
    unsigned int index;
    char m_ip[16];
    unsigned int m_port;
    char *sta_buf, *med_buf, *end_buf;
    Socket_Event_t ret = WisLTE.WaitCheckSocketEvent(m_event, 2);
    switch (ret)
    {
        case SOCKET_CLOSE_EVENT:
            index = atoi(m_event);
            if (WisLTE.CloseSocketService(index)){
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
            if (WisLTE.DeactivateDevAPN(index)){
                DSerial.println("\r\nPlease reconfigure APN!");
            }
            break;
        case SOCKET_INCOMING_CONNECTION_EVENT:
            DSerial.println(m_event);
            *sta_buf = strchr(m_event, ',');
            *sta_buf = '\0';
            index = atoi(m_event);
            *med_buf = strstr(sta_buf + 1, ",\"");
            *end_buf = strstr(sta_buf + 1, "\",");
            *end_buf = '\0';
            strcpy(m_ip, med_buf + 2);
            m_port = atoi(end_buf + 2);
            if(WisLTE.SocketSendData(index, socket, "Hello", m_ip, m_port)){
                DSerial.println("\r\nSocket Send Data successfully");
            }
            break;
        case SOCKET_CONNECTION_FULL_EVENT:
            if(WisLTE.CloseSocketService(comm_socket_index)){
                DSerial.println("\r\nClose Socket function successfully");
            }
            break;
        default:
            break;
    }
}
