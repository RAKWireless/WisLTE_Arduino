#include "WisLTEBG96Serial.h"

WisLTEBG96Serial::WisLTEBG96Serial()
{

}

WisLTEBG96Serial::~WisLTEBG96Serial()
{
    cleanBuffer();
}

WisLTEBG96Serial::WisLTEBG96Serial(Stream &atserial, Stream &dserial) : _atserial(atserial), _dserial(dserial)
{
    _atserial.setTimeout(2000);
    _dserial.setTimeout(2000);
    cleanBuffer();
}

void WisLTEBG96Serial::AT_bypass()
{
    while(_atserial.available()){
        _dserial.write(_atserial.read());
    }
    while(_dserial.available()){
        _atserial.write(_dserial.read());
    }
}

template <class T>
bool WisLTEBG96Serial::AutoATSerialBand(T &_atserial)
{
    _atserial.begin(115200);
    if(sendAndSearch(DEV_AT, RESPONSE_OK, 2)){
        return true;
    }

    for(int i = 0; i < sizeof(Band_list)/sizeof(Band_list[0]); i++){
        _atserial.end();
        _atserial.begin(Band_list[i]);
        for (int j = 0; j < 3; j++){
            if(sendAndSearch(DEV_AT, RESPONSE_OK, 2)){
                return true; 
            }
        }
    }
    _atserial.end();
    return false;
}

bool WisLTEBG96Serial::sendDataAndCheck(const char *data_buf, const char *ok_str, const char *err_str, unsigned int timeout)
{
    delay(100);
    while(_atserial.read()>=0);
    int data_len = strlen(data_buf);
    int send_bytes = _atserial.write(data_buf);
#ifdef UART_DEBUG
    _dserial.print("\r\n");
    _dserial.print(data_buf);
    _dserial.print("\r\n");
    _dserial.print("Send Data len :");
    _dserial.print(send_bytes);
    _dserial.print("\r\n");
#endif
    if(send_bytes == data_len){
        if(readResponseAndSearch(ok_str, err_str, timeout)){
            return true;
        }
    }
    return false;
}


bool WisLTEBG96Serial::sendATcommand(const char *command)
{
    delay(100);
    while(_atserial.read()>=0);
    _atserial.write("AT");
    int cmd_len = strlen(command);
    int send_bytes = _atserial.write(command);
#if defined UART_DEBUG
    _dserial.print("\r\n");
    _dserial.print("-> ");
    _dserial.print("AT");
    _dserial.print(command);
    _dserial.print("\r\n");
#endif
    if (send_bytes != cmd_len){
        return false;
    }
    _atserial.write("\r\n");
    return true;
}


unsigned int WisLTEBG96Serial::readResponseByteToBuffer()
{
    char c = _atserial.read();
    rxBuffer[bufferHead] = c;
    bufferHead = (bufferHead + 1) % RX_BUFFER_LENGTH;
#if defined UART_DEBUG
    if (c == '\n'){
        _dserial.print(c);
        _dserial.print("<- ");
    }else {
        _dserial.print(c);
    }
#endif
    return 1;
}

unsigned int WisLTEBG96Serial::readResponseToBuffer(unsigned int timeout)
{
    unsigned long start_time = millis();
    unsigned int recv_len = 0;
    cleanBuffer();
    while(millis() - start_time < timeout * 1000UL){
        if(serialAvailable()){
            recv_len += readResponseByteToBuffer();
        }
    }
    return recv_len;
}

Cmd_Response_t WisLTEBG96Serial::readResponseAndSearchChr(const char test_chr, unsigned int timeout)
{
    unsigned long start_time = millis();
    unsigned int recv_len = 0;
    cleanBuffer();
    while(millis() - start_time < timeout * 1000UL){
        if (serialAvailable()){
            recv_len += readResponseByteToBuffer();
            if (searchChrBuffer(test_chr)){
                return SUCCESS_RESPONSE;
            }
        } 
    }
    if (recv_len > 0){
        return UNKNOWN_RESPONSE;
    } else {
        return TIMEOUT_RESPONSE;
    }
}

Cmd_Response_t WisLTEBG96Serial::readResponseAndSearch(const char *test_str, unsigned int timeout)
{
    unsigned long start_time = millis();
    unsigned int recv_len = 0;
    cleanBuffer();
    while(millis() - start_time < timeout * 1000UL){
        if (serialAvailable()){
            recv_len += readResponseByteToBuffer();
            if (searchStrBuffer(test_str)){
                return SUCCESS_RESPONSE;
            }
        } 
    }
    if (recv_len > 0){
        return UNKNOWN_RESPONSE;
    } else {
        return TIMEOUT_RESPONSE;
    }
}

Cmd_Response_t WisLTEBG96Serial::readResponseAndSearch(const char *test_str, const char *e_test_str, unsigned int timeout)
{
    unsigned long start_time = millis();
    unsigned int recv_len = 0;
    errorCode = -1;
    cleanBuffer();
    while(millis() - start_time < timeout * 1000UL){
        if (serialAvailable()){
            recv_len += readResponseByteToBuffer();
            if (searchStrBuffer(test_str)){
                return SUCCESS_RESPONSE;
            }else if (searchStrBuffer(e_test_str)){
                start_time = millis();
                while(millis() - start_time < 100UL){
                    if(serialAvailable()){
                        recv_len += readResponseByteToBuffer();
                    }
                }
                char *str_buf = searchStrBuffer(": ");
                if (str_buf != NULL){
                    char err_code[8];
                    strcpy(err_code, str_buf + 2);
                    char *end_buf = strstr(err_code, "\r\n");
                    *end_buf = '\0';
                    errorCode = atoi(err_code);
                }
                return FIAL_RESPONSE;
            }
        } 
    }
    if (recv_len > 0){
        return UNKNOWN_RESPONSE;
    } else {
        return TIMEOUT_RESPONSE;
    }
}

Cmd_Response_t WisLTEBG96Serial::sendAndSearchChr(const char *command, const char test_chr, unsigned int timeout)
{
    for (int i = 0; i < 3; i++){
        if(sendATcommand(command)){
            if (readResponseAndSearchChr(test_chr, timeout) == SUCCESS_RESPONSE){
                return SUCCESS_RESPONSE;
            }
        }
    }
    return TIMEOUT_RESPONSE;
}

Cmd_Response_t WisLTEBG96Serial::sendAndSearch(const char *command, const char *test_str, unsigned int timeout)
{
    for (int i = 0; i < 3; i++){
        if(sendATcommand(command)){
            if (readResponseAndSearch(test_str, timeout) == SUCCESS_RESPONSE){
                return SUCCESS_RESPONSE;
            }
        }
    }
    return TIMEOUT_RESPONSE;
}

Cmd_Response_t WisLTEBG96Serial::sendAndSearch(const char *command, const char *test_str, const char *e_test_str, unsigned int timeout)
{
    Cmd_Response_t resp_status = UNKNOWN_RESPONSE;
    for (int i = 0; i < 3; i++){
        if(sendATcommand(command)){
            resp_status = readResponseAndSearch(test_str, e_test_str, timeout);
            return resp_status;
        }
    }
    return resp_status;
}

char *WisLTEBG96Serial::searchStrBuffer(const char *test_str)
{
    int buf_len = strlen((const char *)rxBuffer);
    if (buf_len < RX_BUFFER_LENGTH){
        return strstr((const char *)rxBuffer,test_str);
    }else {
        return NULL;
    }
}

char *WisLTEBG96Serial::searchChrBuffer(const char test_chr)
{
    int buf_len = strlen((const char *)rxBuffer);
    if (buf_len < RX_BUFFER_LENGTH){
        return strchr((const char *)rxBuffer, test_chr);
    }else {
        return NULL;
    }
}

bool WisLTEBG96Serial::returnErrorCode(int &s_err_code)
{
    s_err_code = -1;
    if(errorCode != -1){
        s_err_code = errorCode;
        return true;
    }
    return false;
}

void WisLTEBG96Serial::cleanBuffer()
{
    memset(rxBuffer,'\0',RX_BUFFER_LENGTH);
    bufferHead = 0;
}

int WisLTEBG96Serial::serialAvailable()
{
    unsigned int ret;
    ret = _atserial.available();
    return ret;
}
