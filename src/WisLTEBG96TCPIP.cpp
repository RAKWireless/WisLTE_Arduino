#include "WisLTEBG96TCPIP.h"

WisLTEBG96TCPIP::WisLTEBG96TCPIP()
{

}

WisLTEBG96TCPIP::~WisLTEBG96TCPIP()
{

}

WisLTEBG96TCPIP::WisLTEBG96TCPIP(Stream &atserial, Stream &dserial):WisLTEBG96Common(atserial,dserial)
{

}

bool WisLTEBG96TCPIP::SetDevAPNParameters(unsigned int pdp_index, Protocol_Type_t type, char *apn, char *usr, char *pwd, Authentication_Methods_t met)
{
    char cmd[64], buf[64];
    strcpy(cmd,APN_PARAMETERS);
    sprintf(buf, "=%d,%d,\"%s\",\"%s\",\"%s\",%d", pdp_index, type, apn, usr, pwd, met);
    strcat(cmd,buf);
    if(sendAndSearch(cmd,RESPONSE_OK,2)){
        return true;
    }
    return false;
}

Cmd_Response_t WisLTEBG96TCPIP::ActivateDevAPN(unsigned int pdp_index)
{
    char cmd[16],buf[8];
    strcpy(cmd,ACTIVATE_APN);
    sprintf(buf, "=%d", pdp_index);
    strcat(cmd,buf);
    return sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 150);
}

bool WisLTEBG96TCPIP::DeactivateDevAPN(unsigned int pdp_index)
{
    char cmd[16],buf[8];
    strcpy(cmd,DEACTIVATE_APN);
    sprintf(buf, "=%d", pdp_index);
    strcat(cmd,buf);
    if(sendAndSearch(cmd,RESPONSE_OK,RESPONSE_ERROR,40)>0){
        return true;
    }
    return false;
}

bool WisLTEBG96TCPIP::GetDevAPNIPAddress(unsigned int pdp_index, char *ip)
{
    char cmd[16],buf[8];
    strcpy(cmd, GET_APN_IP_ADDRESS);
    sprintf(buf, "=%d", pdp_index);
    strcat(cmd,buf);
    if(sendAndSearch(cmd,RESPONSE_OK,2)){
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *sta_buf = searchChrBuffer(',');
        if(strcmp(sta_buf + 1, "0.0.0.0") <= 0){
            return false;
        }
        strcpy(ip, sta_buf + 1);
        return true;
    }
    return false;
}

bool WisLTEBG96TCPIP::InitAPN(unsigned int pdp_index, char *apn, char *usr, char *pwd, char *err_code)
{
    Net_Status_t i_status;
    Cmd_Response_t init_status;
    const char *e_str;
    unsigned long start_time = millis();
    while(!DevSimPIN("",READ_MODE)){
         if (millis() - start_time >= 10*1000UL){
             e_str = "\r\nAPN ERROR: No SIM card detected!\r\n";
             strcpy(err_code, e_str);
             return false;
         }
    }
    start_time = millis();
    while (i_status != REGISTERED && i_status != REGISTERED_ROAMING){
        i_status = DevNetRegistrationStatus();
        if (millis() - start_time >= 120*1000UL){
            e_str = "\r\nAPN ERROR: Can't registered to the Operator network!\r\n";
            strcpy(err_code, e_str);
            return false;
        }
    }
    start_time = millis();
    while(millis() - start_time <= 300*1000UL){
        if (SetDevAPNParameters(pdp_index, IPV4, apn, usr, pwd, PAP_OR_CHAP))
        {
            char i_ip[16];
            if(GetDevAPNIPAddress(pdp_index, i_ip)){
                sprintf(err_code, "\r\nAPN OK: The IP address is %s\r\n", i_ip);
                return true;
            }else{
                init_status = ActivateDevAPN(pdp_index);
                if (init_status == TIMEOUT_RESPONSE){
                    e_str = "\r\nAPN ERROR: Please reset your device!\r\n";
                    strcpy(err_code, e_str);
                    return false;
                }
            }
        }
        e_str = "\r\nAPN ERROR: Activate APN file!\r\n";
        strcpy(err_code, e_str);
    }
    return false;
}

bool WisLTEBG96TCPIP::OpenSocketService(unsigned int pdp_index, unsigned int socket_index, Socket_Type_t socket, char *ip, unsigned int port, unsigned int local_port, Access_Mode_t mode)
{
    char cmd[128],buf[128];
    strcpy(cmd,OPEN_SOCKET);
    switch(socket)
    {
        case TCP_CLIENT:
            sprintf(buf, "=%d,%d,\"TCP\",\"%s\",%d,%d,%d", pdp_index, socket_index, ip, port, local_port, mode);
            break;
        case TCP_SEVER:
            sprintf(buf, "=%d,%d,\"TCP LISTENER\",\"%s\",%d,%d,%d", pdp_index, socket_index, ip, port, local_port, mode);
            break;
        case UDP_CLIENT:
            sprintf(buf, "=%d,%d,\"UDP\",\"%s\",%d,%d,%d", pdp_index, socket_index, ip, port, local_port, mode);
            break;
        case UDP_SEVER:
            sprintf(buf, "=%d,%d,\"UDP SERVICE\",\"%s\",%d,%d,%d", pdp_index, socket_index, ip, port, local_port, mode);
            break;
        default:
            return false;
    }
    strcat(cmd,buf);
    switch(mode)
    {
        case BUFFER_MODE:
        case DIRECT_PUSH_MODE:
            if (sendAndSearch(cmd, OPEN_SOCKET, RESPONSE_ERROR, 150) > 0){
                unsigned long start_time = millis();
                while(millis() - start_time < 200UL){
                    if (serialAvailable()){
                        readResponseByteToBuffer();
                    }
                }
                errorCode = -1;
                char *sta_buf = searchChrBuffer(',');
                if(atoi(sta_buf + 1) == 0){
                    return true;
                }else {
                    errorCode = atoi(sta_buf + 1);
                }
            }
            break;
        case TRANSPARENT_MODE:
            if (sendAndSearch(cmd, RESPONSE_CONNECT, RESPONSE_ERROR, 150) > 0){;
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

bool WisLTEBG96TCPIP::CloseSocketService(unsigned int socket_index)
{
    char cmd[16],buf[8];
    strcpy(cmd,CLOSE_SOCKET);
    sprintf(buf,"=%d",socket_index);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 150)){
        return true;
    }
    return false;
}

bool WisLTEBG96TCPIP::SocketSendData(unsigned int socket_index, Socket_Type_t socket, char *data_buf, char *ip, unsigned int port)
{
    char cmd[64],buf[64];
    strcpy(cmd, SOCKET_SEND_DATA);
    switch(socket)
    {
        case TCP_CLIENT:
        case TCP_SEVER:
        case UDP_CLIENT:
            sprintf(buf,"=%d,%d",socket_index,strlen(data_buf));
            break;
        case UDP_SEVER:
            sprintf(buf,"=%d,%d,\"%s\",%d",socket_index,strlen(data_buf),ip,port);
            break;
        default:
        return false;
    }
    strcat(cmd,buf);
    if(sendAndSearchChr(cmd, '>', 2)){
        if(sendDataAndCheck(data_buf, RESPONSE_SEND_OK, RESPONSE_SEND_FAIL, 10)){
            return true;
        }
    }
    return false;
}

bool WisLTEBG96TCPIP::SocketRecvData(unsigned int socket_index, unsigned int recv_len, Socket_Type_t socket, char *recv_buf)
{
    char cmd[16],buf[16];
    strcpy(cmd, SOCKET_READ_DATA);
    switch(socket)
    {
        case TCP_CLIENT:
        case TCP_SEVER:
        case UDP_CLIENT:
            sprintf(buf,"=%d,%d",socket_index,recv_len);
            break;
        case UDP_SEVER:
            sprintf(buf,"=%d",socket_index);
            break;
        default:
            return false;
    }
    strcat(cmd,buf);
    if(sendAndSearch(cmd, RESPONSE_CRLF_OK, RESPONSE_ERROR, 10)){
        char *sta_buf = searchStrBuffer(": "); 
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(recv_buf, sta_buf + 2);
        return true;
    }
    return false;
}

bool WisLTEBG96TCPIP::SocketSendHEXData(unsigned int socket_index, char *hex_buf)
{
    char cmd[256],buf[256];
    strcpy(cmd, SOCKET_SEND_HEX_DATA);
    sprintf(buf, "=%d,\"%s\"", socket_index, hex_buf);
    strcat(cmd,buf);
    if(sendAndSearch(cmd, RESPONSE_SEND_OK, RESPONSE_SEND_FAIL, 2)){
        return true;
    }
    return false;
}

bool WisLTEBG96TCPIP::SwitchAccessModes(unsigned int socket_index, Access_Mode_t mode)
{
    char cmd[16],buf[16];
    strcpy(cmd, DATA_ACCESS_MODES);
    sprintf(buf, "=%d,%d", socket_index, mode);
    strcat(cmd,buf);
    if (mode == TRANSPARENT_MODE){
        if(sendAndSearch(cmd, RESPONSE_CONNECT, RESPONSE_ERROR, 2)){
            return true;
        }
    }else {
        if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
            return true;
        }
    }
    return false;
}

bool WisLTEBG96TCPIP::DevPingFunction(unsigned int socket_index, char *host)
{
    char cmd[128],buf[64];
    strcpy(cmd, PING_FUNCTION);
    sprintf(buf, "=%d,\"%s\"", socket_index, host);
    strcat(cmd,buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        if(readResponseToBuffer(20)){
            char *sta_buf = searchStrBuffer(PING_FUNCTION);
            char ping_data[256];
            strcpy(ping_data, sta_buf);
            char *p[6]; int i = 0;
            p[0] = strtok(ping_data, "\r\n\r\n");
            while(p[i] != NULL){
                i++;
                p[i] = strtok(NULL,"\r\n\r\n");
            }
            p[i] = '\0';
            for (int j = 0; j <= 4; j++)
            {
                char *cs_buf = strstr(p[j],": ");
                char *ce_buf = strchr(p[j],',');
                if(ce_buf == NULL){
                    errorCode = atoi(cs_buf + 2);
                }else{
                    *ce_buf = '\0';
                    int code = atoi(cs_buf + 2);
                    if (code == 0){
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool WisLTEBG96TCPIP::DevNTPFunction(unsigned int socket_index, char *ntp_ip, unsigned int port, char *time)
{
    char cmd[64],buf[64];
    strcpy(cmd, NTP_FUNCTION);
    sprintf(buf, "=%d,\"%s\",%d", socket_index, ntp_ip, port);
    strcat(cmd, buf);
    if (sendAndSearch(cmd, NTP_FUNCTION, RESPONSE_ERROR, 125)){
        unsigned long start_time = millis();
        while(millis() - start_time < 200UL){
            if (serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchStrBuffer(": ");
        strcpy(time, sta_buf + 2);
        return true;
    }
    return false;
}

bool WisLTEBG96TCPIP::ConfigDNSServer(unsigned int socket_index, char *primary_addr, char *secondary_addr)
{
    char cmd[128],buf[128];
    strcpy(cmd, CONFIGURE_DNS_SERVER);
    sprintf(buf, "=%d,\"%s\",\"%s\"", socket_index, primary_addr, secondary_addr);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return true;
    }
    return false;
}

bool WisLTEBG96TCPIP::DevDNSFunction(unsigned int socket_index, char *domain_name, char *ip)
{
    char cmd[128],buf[128];
    strcpy(cmd, DNS_FUNCTION);
    sprintf(buf, "=%d,\"%s\"", socket_index, domain_name);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, "+QIURC", RESPONSE_ERROR, 60)){
        unsigned long start_time = millis();
        while(millis() - start_time < 200UL){
            if (serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchStrBuffer(": ");
        strcpy(ip, sta_buf + 2);
        return true;
    }
    return false;
}

bool WisLTEBG96TCPIP::QueryLastErrorCode(char *err_code)
{
    char cmd[16];
    strcpy(cmd, QUERY_ERROR_CODE);
    if(sendAndSearch(cmd, RESPONSE_OK, 2)){
        char *sta_buf = searchStrBuffer(": ");
        strcpy(err_code, sta_buf + 2);
        return true;
    }
    return false;
}

Socket_Event_t WisLTEBG96TCPIP::WaitCheckSocketEvent(char *event, unsigned int timeout)
{
    if(readResponseAndSearch(RECV_SOCKET_EVENT, timeout)){
        unsigned long start_time = millis();
        while(millis() - start_time < 200UL){
            if (serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchChrBuffer(',');
        strcpy(event, sta_buf + 1);
        if(searchStrBuffer("closed")){
            return SOCKET_CLOSE_EVENT;
        }else if (searchStrBuffer("recv")){
            return SOCKET_RECV_DATA_EVENT;
        }else if (searchStrBuffer("incoming full")){
            return SOCKET_CONNECTION_FULL_EVENT;
        }else if (searchStrBuffer("incoming")){
            return SOCKET_INCOMING_CONNECTION_EVENT;
        }else if (searchStrBuffer("pdpdeact")){
            return SOCKET_PDP_DEACTIVATION_EVENT;
        }
    }
    return 0;
}