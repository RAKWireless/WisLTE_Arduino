#include "WisLTEBG96Common.h"

WisLTEBG96Common::WisLTEBG96Common()
{

}

WisLTEBG96Common::~WisLTEBG96Common()
{

}

WisLTEBG96Common::WisLTEBG96Common(Stream &atserial, Stream &dserial):WisLTEBG96Serial(atserial,dserial)
{

}

bool WisLTEBG96Common::InitModule()
{
#ifdef WISLTE_BOARD
    pinMode(POWKEY_PIN, OUTPUT);
    delay(100);
    digitalWrite(POWKEY_PIN, LOW);
    delay(200);
    digitalWrite(POWKEY_PIN, HIGH);
    if(readResponseToBuffer(5)){
        if(searchStrBuffer(RESPONSE_READY)){
            return true;
        }
    }
    delay(1000);
    digitalWrite(POWKEY_PIN, LOW);
    delay(2000);
    digitalWrite(POWKEY_PIN, HIGH);
    if(readResponseToBuffer(5)){
        if(searchStrBuffer(RESPONSE_POWER_DOWN)){
            delay(10000);
        }
    }
#endif
#ifdef WISCELLULAR_BOARD
    pinMode(RESET_PIN, OUTPUT);
    digitalWrite(RESET_PIN, LOW);
    pinMode(POWKEY_PIN, OUTPUT);
    delay(100);
    digitalWrite(POWKEY_PIN, HIGH);
    delay(200);
    digitalWrite(POWKEY_PIN, LOW);
    if(readResponseToBuffer(5)){
        if(searchStrBuffer(RESPONSE_READY)){
            return true;
        }
    }
    delay(1000);
    digitalWrite(POWKEY_PIN, HIGH);
    delay(2000);
    digitalWrite(POWKEY_PIN, LOW);
    if(readResponseToBuffer(5)){
        if(searchStrBuffer(RESPONSE_POWER_DOWN)){
            delay(10000);
        }
    }
#endif
    return false;
}

bool WisLTEBG96Common::SetDevCommandEcho(bool echo)
{
    const char *cmd;
    if (echo == true){
        cmd = "E1";
    }else{
        cmd = "E0";
    }
    if(sendAndSearch(cmd,RESPONSE_OK,2)){
        return true;
    }
    return false;
}

bool WisLTEBG96Common::GetDevInformation(char *inf)
{
    if (sendAndSearch(DEV_INFORMATION,RESPONSE_OK,2)){
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(inf,rxBuffer);
        return true;
    }
    return false;
}

bool WisLTEBG96Common::GetDevVersion(char *ver)
{
    if (sendAndSearch(DEV_VERSION,RESPONSE_OK,2)){
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(ver, rxBuffer);
        return true;
    }
    return false;
}

bool WisLTEBG96Common::GetDevIMEI(char *imei)
{
    if (sendAndSearch(DEV_IMEI,RESPONSE_OK,2)){
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(imei, rxBuffer);
        return true;
    }
    return false;
}

Cmd_Response_t WisLTEBG96Common::SetDevFunctionality(Functionality_t mode)
{
    char cmd[16];
    Cmd_Response_t fun_status;
    strcpy(cmd, DEV_FUN_LEVEL);
    switch(mode)
    {
        case MINIMUM_FUNCTIONALITY:
            strcat(cmd,"=0");
            break;
        case FULL_FUNCTIONALITY:
            strcat(cmd,"=1");
            break;
        case DISABLE_RF:
            strcat(cmd,"=4");
            break;
        default:
            return UNKNOWN_RESPONSE;
    }
    fun_status = sendAndSearch(cmd,RESPONSE_OK,RESPONSE_ERROR,15);
    return fun_status;
}

bool WisLTEBG96Common::DevLocalRate(unsigned long &rate, Cmd_Status_t status)
{
    char cmd[16];
    strcpy(cmd, DEV_LOCAL_RATE);
    if (status == READ_MODE){
        strcat(cmd,"?");
        if(sendAndSearch(cmd,RESPONSE_OK,2)){
            char *sta_buf = searchStrBuffer(": ");
            char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
            *end_buf = '\0';
            rate = atol(sta_buf + 2);
            return true;
        }
    }else if (status == WRITE_MODE){
        for (int i = 0; i < sizeof(Band_list)/sizeof(Band_list[0]); i++){
            if (rate == Band_list[i]){
                char buf[16];
                sprintf(buf, "=%ld;&W", rate);
                strcat(cmd, buf);
                if (sendAndSearch(cmd,RESPONSE_OK,2)){
                    return true;
                }
            }
        }
    }
    return false;
}

bool WisLTEBG96Common::GetDevSimIMSI(char *imsi)
{
    if (sendAndSearch(DEV_SIM_IMSI,RESPONSE_OK,2)){
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(imsi, rxBuffer);
        return true;
    }
    return false;
}

bool WisLTEBG96Common::DevSimPIN(char *pin, Cmd_Status_t status)
{
    char cmd[16];
    strcpy(cmd, DEV_SIM_PIN);
    if (status == READ_MODE){
        strcat(cmd,"?");
        if (sendAndSearch(cmd,"READY",2)){
            //pin = "READY";
            return true;
        }
    }else if (status == WRITE_MODE){
        char buf[16];
        sprintf(buf,"=\"%s\"",pin);
        strcat(cmd,buf);
        if(sendAndSearch(cmd,RESPONSE_OK,2)){
            return true;
        }
    }
    return false;
}

bool WisLTEBG96Common::GetDevSimICCID(char *iccid)
{
    if (sendAndSearch(DEV_SIM_ICCID,RESPONSE_OK,2)){
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *sta_buf = searchStrBuffer(": ");
        strcpy(iccid, sta_buf + 2);
        return true;
    }
    return false;
}

Net_Status_t WisLTEBG96Common::DevNetRegistrationStatus()
{
    char cmd[16];
    Net_Status_t n_status = NOT_REGISTERED;
    strcpy(cmd, DEV_NET_STATUS_G);
    strcat(cmd,"?");
    if (sendAndSearch(cmd, RESPONSE_OK, 2)){
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *sta_buf = searchChrBuffer(',');
        n_status = atoi(sta_buf + 1);
        switch (n_status)
        {
            case REGISTERED:
            case REGISTERED_ROAMING:
                return n_status;
            default:
                break;
        }
    }

    strcpy(cmd, DEV_EPS_NET_STATUS);
    strcat(cmd, "?");
    if (sendAndSearch(cmd, RESPONSE_OK, 2)){
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *sta_buf = searchChrBuffer(',');
        n_status = atoi(sta_buf + 1);
        switch (n_status)
        {
            case REGISTERED:
            case REGISTERED_ROAMING:
                return n_status;
            default:
                break;
        }
    }
    return n_status;
}

bool WisLTEBG96Common::GetDevNetSignalQuality(unsigned int &rssi)
{
    if (sendAndSearch(DEV_NET_RSSI,RESPONSE_OK,2)){
        char *sta_buf = searchStrBuffer(": ");
        char *end_buf = searchChrBuffer(',');
        *end_buf = '\0';
        rssi = atoi(sta_buf + 2);
        return true;
    }
    return false;
}

Cmd_Response_t WisLTEBG96Common::ScanOperatorNetwork(char *net)
{
    char cmd[16];
    Cmd_Response_t scan_status;
    strcpy(cmd, DEV_NET_OPERATOR);
    strcat(cmd,"=?");
    scan_status = sendAndSearch(cmd,RESPONSE_OK,RESPONSE_ERROR,180);
    if (scan_status == SUCCESS_RESPONSE){
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *sta_buf = searchStrBuffer(": ");
        strcpy(net,sta_buf + 2);
    }else if(scan_status == FIAL_RESPONSE){
        char *sta_buf = searchStrBuffer(": ");
        strcpy(net,sta_buf + 2);
    }
    return scan_status;
}

Cmd_Response_t WisLTEBG96Common::DevOperatorNetwork(unsigned int &mode, unsigned int &format, char *oper, Net_Type_t &act, Cmd_Status_t status)
{
    char cmd[16];
    Cmd_Response_t oper_status = UNKNOWN_RESPONSE;
    strcpy(cmd,DEV_NET_OPERATOR);
    if(status == READ_MODE){
        strcat(cmd,"?");
        oper_status = sendAndSearch(cmd,RESPONSE_OK,RESPONSE_ERROR,5);
        if(oper_status == SUCCESS_RESPONSE){
            char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
            *end_buf = '\0';
            char *sta_buf = searchStrBuffer(": ");
            char message[64]; char *p[5]; int i = 0;
            strcpy(message,sta_buf + 2);
            p[0]= strtok(message,",");
            while(p[i] != NULL){
                i++;
                p[i] = strtok(NULL,",");
            }
            p[i] = '\0';
            mode = atoi(p[0]);
            format = atoi(p[1]);
            strcpy(oper,p[2]);
            act = atoi(p[3]);
        }
    }else if(status == WRITE_MODE){
        char buf[32]; 
        sprintf(buf,"=%d,%d,\"%s\",%d",mode,format,oper,act);
        strcat(cmd,buf);
        oper_status = sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 30);
    }
    return oper_status;
}

bool WisLTEBG96Common::GetDevNetworkInformation(char *type, char *oper, char *band, char *channel)
{
    if (sendAndSearch(DEV_NET_INFORMATION,RESPONSE_OK,2)){
        char *sta_buf = searchStrBuffer(": ");
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char message[64]; char *p[5]; int i = 0;
        strcpy(message,sta_buf + 2);
        p[0] = strtok(message,",");
        while(p[i] != NULL){
            i++;
            p[i] = strtok(NULL,",");
        }
        p[i] = '\0';
        strcpy(type, p[0]);
        strcpy(oper, p[1]);
        strcpy(band, p[2]);
        strcpy(channel, p[3]);
        return true;
    }
    return false;
}

bool WisLTEBG96Common::DevNetPacketCounter(unsigned long &send_bytes, unsigned long &recv_bytes, bool clean)
{
    char cmd[16];
    strcpy(cmd,DEV_NET_PACKET_COUNTER);
    strcat(cmd,"?");
    if(sendAndSearch(cmd,RESPONSE_OK,2)){
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *med_buf = searchChrBuffer(',');
        *med_buf = '\0';
        char *sta_buf = searchStrBuffer(": ");
        send_bytes = atol(sta_buf + 2);
        recv_bytes = atol(med_buf + 1);
        if (clean == true){
            memset(cmd,'\0',16);
            strcpy(cmd,DEV_NET_PACKET_COUNTER);
            strcat(cmd,"=0");
            if(sendAndSearch(cmd,RESPONSE_OK,2)){
                return true;
            }else {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool WisLTEBG96Common::DevPowerDown()
{
    char cmd[16];
    strcpy(cmd,DEV_POWER_DOWN);
    strcat(cmd,"=1");
    if(sendAndSearch(cmd,RESPONSE_POWER_DOWN,2)){
        return true;
    }
    return false;
}

bool WisLTEBG96Common::DevClock(char *d_clock, Cmd_Status_t status)
{
    char cmd[32];
    strcpy(cmd,DEV_CLOCK);
    if (status == READ_MODE){
        strcat(cmd,"?");
        if (sendAndSearch(cmd,RESPONSE_OK,2)){
            char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
            *end_buf = '\0';
            char *sta_buf = searchStrBuffer(": ");
            strcpy(d_clock,sta_buf + 2);
            return true;
        }
    }else if (status == WRITE_MODE){
        char buf[32];
        sprintf(buf,"=\"%s\"",d_clock);
        strcat(cmd,buf);
        if (sendAndSearch(cmd,RESPONSE_OK,RESPONSE_ERROR,2)){
            return true;
        }
    }
    return false;
}