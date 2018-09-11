#include "WisLTEBG96SSL.h"

WisLTEBG96SSL::WisLTEBG96SSL()
{

}

WisLTEBG96SSL::~WisLTEBG96SSL()
{
    cleanBuffer();
}

WisLTEBG96SSL::WisLTEBG96SSL(Stream &atserial, Stream &dserial) : WisLTEBG96FILE(atserial, dserial)
{

}

bool WisLTEBG96SSL::SetSSLParameters(unsigned int ssl_index, SSL_Version_t s_version, SSL_Cipher_Suites_t s_ciper, unsigned int negotiation_time)
{
    char cmd[64],buf[32];
    strcpy(cmd, SSL_CONFIG_PARAMETER);
    sprintf(buf, "=\"sslversion\",%d,%d", ssl_index, s_version);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
        memset(cmd, '\0', 64);
        memset(buf, '\0', 32);
        strcpy(cmd, SSL_CONFIG_PARAMETER);
        switch (s_ciper)
        {
            case TLS_RSA_WITH_AES_256_CBC_SHA:
                sprintf(buf, "=\"ciphersuite\",%d,0X0035", ssl_index);
                break;
            case TLS_RSA_WITH_AES_128_CBC_SHA:
                sprintf(buf, "=\"ciphersuite\",%d,0X002F", ssl_index);
                break;
            case TLS_RSA_WITH_RC4_128_SHA:
                sprintf(buf, "=\"ciphersuite\",%d,0X0005", ssl_index);
                break;
            case TLS_RSA_WITH_RC4_128_MD5:
                sprintf(buf, "=\"ciphersuite\",%d,0X0004", ssl_index);
                break;
            case TLS_RSA_WITH_3DES_EDE_CBC_SHA:
                sprintf(buf, "=\"ciphersuite\",%d,0X000A", ssl_index);
                break;
            case TLS_RSA_WITH_AES_256_CBC_SHA256:
                sprintf(buf, "=\"ciphersuite\",%d,0X003D", ssl_index);
                break;
            case TLS_ECDHE_RSA_WITH_RC4_128_SHA:
                sprintf(buf, "=\"ciphersuite\",%d,0XC011", ssl_index);
                break;
            case TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA:
                sprintf(buf, "=\"ciphersuite\",%d,0XC012", ssl_index);
                break;
            case TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA:
                sprintf(buf, "=\"ciphersuite\",%d,0XC013", ssl_index);
                break;
            case TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA:
                sprintf(buf, "=\"ciphersuite\",%d,0XC014", ssl_index);
                break;
            case TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256:
                sprintf(buf, "=\"ciphersuite\",%d,0C027", ssl_index);
                break;
            case TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384:
                sprintf(buf, "=\"ciphersuite\",%d,0XC028", ssl_index);
                break;
            case TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256:
                sprintf(buf, "=\"ciphersuite\",%d,0XC02F", ssl_index);
                break;
            default:
                sprintf(buf, "=\"ciphersuite\",%d,0XFFFF", ssl_index);
                break;
        }
        strcat(cmd, buf);
        if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
            memset(cmd, '\0', 64);
            memset(buf, '\0', 32);
            strcpy(cmd, SSL_CONFIG_PARAMETER);
            sprintf(buf, "=\"negotiatetime\",%d,%d", ssl_index, negotiation_time);
            strcat(cmd, buf);
            if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
                return true;
            }
        }
    }
    return false;
}

bool WisLTEBG96SSL::SetSSLCertificate(unsigned int ssl_index, char *ca_cert_path, char *client_cert_path, char *client_key_path, bool validity_check)
{
    char cmd[64],buf[32];
    strcpy(cmd, SSL_CONFIG_PARAMETER);
    if(ca_cert_path == "" && client_cert_path == "" && client_key_path == ""){
        sprintf(buf, "=\"seclevel\",%d,0", ssl_index);
        strcat(cmd, buf);
        if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
            return true;
        }
    }else if(ca_cert_path != "" && client_cert_path == "" && client_key_path == ""){
        sprintf(buf, "=\"seclevel\",%d,1", ssl_index);
        strcat(cmd, buf);
        if(!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
            return false;
        }
        memset(cmd, '\0', 64);
        memset(buf, '\0', 32);
        strcpy(cmd, SSL_CONFIG_PARAMETER);
        sprintf(buf, "=\"cacert\",%d,\"%s\"", ssl_index, ca_cert_path);
        strcat(cmd, buf);
        if(!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
            return false;
        }
        memset(cmd, '\0', 64);
        memset(buf, '\0', 32);
        strcpy(cmd, SSL_CONFIG_PARAMETER);
        if(validity_check){
            sprintf(buf, "=\"ignorelocaltime\",%d,1", ssl_index);
            strcat(cmd, buf);
            if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
                return true;
            }
        }else{
            sprintf(buf, "=\"ignorelocaltime\",%d,0", ssl_index);
            strcat(cmd, buf);
            if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
                return true;
            }
        }
            
    }else if(ca_cert_path != "" && client_cert_path != "" && client_key_path != ""){
        sprintf(buf, "=\"seclevel\",%d,2", ssl_index);
        strcat(cmd, buf);
        if(!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
            return false;
        }
        memset(cmd, '\0', 64);
        memset(buf, '\0', 32);
        strcpy(cmd, SSL_CONFIG_PARAMETER);
        sprintf(buf, "=\"cacert\",%d,\"%s\"", ssl_index, ca_cert_path);
        strcat(cmd, buf);
        if(!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
            return false;
        }
        memset(cmd, '\0', 64);
        memset(buf, '\0', 32);
        strcpy(cmd, SSL_CONFIG_PARAMETER);
        sprintf(buf, "=\"clientcert\",%d,\"%s\"", ssl_index, client_cert_path);
        strcat(cmd, buf);
        if(!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
            return false;
        }
        memset(cmd, '\0', 64);
        memset(buf, '\0', 32);
        strcpy(cmd, SSL_CONFIG_PARAMETER);
        sprintf(buf, "=\"clientkey\",%d,\"%s\"", ssl_index, client_key_path);
        strcat(cmd, buf);
        if(!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
            return false;
        }
        memset(cmd, '\0', 64);
        memset(buf, '\0', 32);
        strcpy(cmd, SSL_CONFIG_PARAMETER);
        if(validity_check){
            sprintf(buf, "=\"ignorelocaltime\",%d,1", ssl_index);
            strcat(cmd, buf);
            if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
                return true;
            }
        }else{
            sprintf(buf, "=\"ignorelocaltime\",%d,0", ssl_index);
            strcat(cmd, buf);
            if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
                return true;
            }
        }
    }
    return false;
}

bool WisLTEBG96SSL::InitSSL(unsigned int ssl_index, char *ca_cert, char *client_cert, char *client_key, char *err_code)
{
    unsigned long start_time = millis();
    const char *e_str;
    int f_err_code;
    if (!SetSSLParameters(ssl_index, ALL_VERSION, SUPPORT_ALL_ABOVE, 300)){
        e_str = "\r\nSSL ERROR: An error occurred while setting the ssl parameter.\r\n";
        strcpy(err_code, e_str);
        return false;
    }

    if (ca_cert == "" && client_cert == "" && client_key == ""){
        if (SetSSLCertificate(ssl_index, "", "", "", false)){
            e_str = "\r\nSSL OK: The ssl were successfully initialized.\r\n";
            strcpy(err_code, e_str);
            return true;
        }
        e_str = "\r\nSSL ERROR: An error occurred while setting the ssl certificate.\r\n";
        strcpy(err_code, e_str);
    }else if (ca_cert != "" && client_cert == "" && client_key == ""){
        while (!UploadFiles(ssl_ca_cert_name, ca_cert)){
            if(returnErrorCode(f_err_code)){
                if (f_err_code == 407){
                    start_time = millis();
                    while (!DeleteFiles(ssl_ca_cert_name)){
                        if(millis() - start_time >= 10*1000UL){
                            e_str = "\r\nSSL ERROR: The ssl ca cert file exists. An error occurred while deleting the original file during the re-upload process.\r\n";
                            strcpy(err_code, e_str);
                            return false;
                        }
                    }
                }
            }else if(millis() - start_time >= 30*1000UL){
                sprintf(e_str, "\r\nSSL ERROR: Error uploading file, error code: %d ,Please check the corresponding documentation for details.\r\n", f_err_code);
                strcpy(err_code, e_str);
                return false;
            }
        }
        start_time = millis();
        while (!SetSSLCertificate(ssl_index, ssl_ca_cert_name, "", "", false)){
            if (millis() - start_time >= 30 * 1000UL){
                e_str = "\r\nSSL ERROR: An error occurred while setting the ssl certificate.\r\n";
                strcpy(err_code, e_str);
                return false;
            }
        }
        e_str = "\r\nSSL OK: The ssl were successfully initialized.\r\n";
        strcpy(err_code, e_str);
        return true;
    }else if (ca_cert != "" && client_cert != "" && client_key != ""){
        while (!UploadFiles(ssl_ca_cert_name, ca_cert)){
            if(returnErrorCode(f_err_code)){
                if (f_err_code == 407){
                    start_time = millis();
                    while (!DeleteFiles(ssl_ca_cert_name)){
                        if(millis() - start_time >= 10*1000UL){
                            e_str = "\r\nSSL ERROR: The ssl ca cert file exists. An error occurred while deleting the original file during the re-upload process.\r\n";
                            strcpy(err_code, e_str);
                            return false;
                        }
                    }
                }
            }else if(millis() - start_time >= 30*1000UL){
                sprintf(e_str, "\r\nSSL ERROR: Error uploading file, error code: %d ,Please check the corresponding documentation for details.\r\n", f_err_code);
                strcpy(err_code, e_str);
                return false;
            }
        }
        start_time = millis();
        while (!UploadFiles(ssl_client_cert_name, client_cert)){
            if(returnErrorCode(f_err_code)){
                if (f_err_code == 407){
                    start_time = millis();
                    while (!DeleteFiles(ssl_client_cert_name)){
                        if(millis() - start_time >= 10*1000UL){
                            e_str = "\r\nSSL ERROR: The ssl ca cert file exists. An error occurred while deleting the original file during the re-upload process.\r\n";
                            strcpy(err_code, e_str);
                            return false;
                        }
                    }
                }
            }else if(millis() - start_time >= 30*1000UL){
                sprintf(e_str, "\r\nSSL ERROR: Error uploading file, error code: %d ,Please check the corresponding documentation for details.\r\n", f_err_code);
                strcpy(err_code, e_str);
                return false;
            }
        }
        start_time = millis();
        while (!UploadFiles(ssl_client_key_name, client_key)){
            if(returnErrorCode(f_err_code)){
                if (f_err_code == 407){
                    start_time = millis();
                    while (!DeleteFiles(ssl_client_key_name)){
                        if(millis() - start_time >= 10*1000UL){
                            e_str = "\r\nSSL ERROR: The ssl ca cert file exists. An error occurred while deleting the original file during the re-upload process.\r\n";
                            strcpy(err_code, e_str);
                            return false;
                        }
                    }
                }
            }else if(millis() - start_time >= 30*1000UL){
                sprintf(e_str, "\r\nSSL ERROR: Error uploading file, error code: %d ,Please check the corresponding documentation for details.\r\n", f_err_code);
                strcpy(err_code, e_str);
                return false;
            }
        }
        start_time = millis();
        while (!SetSSLCertificate(ssl_index, ssl_ca_cert_name, ssl_client_cert_name, ssl_client_key_name, false)){
            if (millis() - start_time >= 30 * 1000UL){
                e_str = "\r\nSSL ERROR: An error occurred while setting the ssl certificate.\r\n";
                strcpy(err_code, e_str);
                return false;
            }
        }
        e_str = "\r\nSSL OK: The ssl were successfully initialized.\r\n";
        strcpy(err_code, e_str);
        return true;
    }
    return false;
}

bool WisLTEBG96SSL::OpenSSLSocket(unsigned int pdp_index, unsigned int ssl_index, unsigned int socket_index, char *ip, unsigned int port, Access_Mode_t mode)
{
    char cmd[64],buf[64];
    strcpy(cmd, SSL_OPEN_SOCKET);
    sprintf(buf, "=%d,%d,%d,\"%s\",%d,%d", pdp_index, ssl_index, socket_index, ip, port, mode);
    strcat(cmd, buf);
    switch(mode)
    {
        case BUFFER_MODE:
        case DIRECT_PUSH_MODE:
            if (sendAndSearch(cmd, SSL_OPEN_SOCKET, RESPONSE_ERROR, 150) > 0){
                unsigned long start_time = millis();
                while(millis() - start_time < 200UL){
                    if (serialAvailable()){
                        readResponseByteToBuffer();
                    }
                }
                char *sta_buf = searchChrBuffer(',');
                errorCode = -1;
                if(atoi(sta_buf + 1) == 0){
                    return true;
                }else{
                    errorCode = atoi(sta_buf + 1);
                }
            }
            break;
        case TRANSPARENT_MODE:
            if (sendAndSearch(cmd, RESPONSE_CONNECT, RESPONSE_ERROR, 150) > 0){
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

bool WisLTEBG96SSL::SSLSocketSendData(unsigned int socket_index, char *ssl_send_data)
{
    char cmd[16], buf[8];
    strcpy(cmd, SSL_SEND_DATA);
    sprintf(buf, "=%d,%d", socket_index, strlen(ssl_send_data));
    strcat(cmd, buf);
    if(sendAndSearchChr(cmd, '>', 2)){
        if(sendDataAndCheck(ssl_send_data, RESPONSE_SEND_OK, RESPONSE_SEND_FAIL, 30)){
            return true;
        }
    }
    return false;
}

bool WisLTEBG96SSL::SSLSocketRecvData(unsigned int socket_index, unsigned int ssl_recv_len, char *ssl_recv_data)
{
    char cmd[16],buf[16];
    strcpy(cmd, SSL_READ_DATA);
    sprintf(buf, "=%d,%d", socket_index, ssl_recv_len);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_CRLF_OK, RESPONSE_ERROR, 30)){
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *sta_buf = searchStrBuffer(": "); 
        strcpy(ssl_recv_data, sta_buf + 2);
        return true;
    }
    return false;
}

bool WisLTEBG96SSL::CloseSSLSocket(unsigned int socket_index)
{
    char cmd[16],buf[16];
    strcpy(cmd, SSL_CLOSE_SOCKET);
    sprintf(buf, "=%d", socket_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 150)){
        return true;
    }
    return false;
}

bool WisLTEBG96SSL::QuerySSLSocketStatus(unsigned int socket_index, char *ssl_status)
{
    char cmd[16],buf[16];
    strcpy(cmd, SSL_QUERY_STATUS);
    sprintf(buf, "=%d", socket_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, 10)){
        char *sta_buf = searchStrBuffer(": ");
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(ssl_status, sta_buf + 2);
        return true;
    }
    return false;
}

SSL_Socket_Event_t WisLTEBG96SSL::WaitCheckSSLSocketEvent(char *event, unsigned int timeout)
{
    if(readResponseAndSearch(SSL_SOCKET_EVENT, timeout)){
        unsigned long start_time = millis();
        while(millis() - start_time < 200UL){
            if (serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchChrBuffer(',');
        strcpy(event, sta_buf + 1);
        if(searchStrBuffer("closed")){
            return SSL_SOCKET_CLOSE_EVENT;
        }else if (searchStrBuffer("recv")){
            return SSL_SOCKET_RECV_EVENT;
        }
    }
    return 0;
}