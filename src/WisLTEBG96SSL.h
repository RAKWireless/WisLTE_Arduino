/*
 * A library for RAK WisLTE BG96 Development board
 * This file is about the BG96 SSL function
 * 
 * Copyright (c) 2018 RAKWireless technology inc.
 * @Author       : Chace
 * @Create time  : 08/16/2018
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
#ifndef _WISLTEBG96SSL_H_
#define _WISLTEBG96SSL_H_

#include "WisLTEBG96FILE.h"

typedef enum ssl_version{
    SSL_3_0 = 0,
    TLS_1_0 = 1,
    TLS_1_1 = 2,
    TLS_1_2 = 3,
    ALL_VERSION = 4,
}SSL_Version_t;

typedef enum ssl_cipher_suites{
    TLS_RSA_WITH_AES_256_CBC_SHA = 0,
    TLS_RSA_WITH_AES_128_CBC_SHA = 1,
    TLS_RSA_WITH_RC4_128_SHA = 2,
    TLS_RSA_WITH_RC4_128_MD5 = 3,
    TLS_RSA_WITH_3DES_EDE_CBC_SHA = 4,
    TLS_RSA_WITH_AES_256_CBC_SHA256 = 5,
    TLS_ECDHE_RSA_WITH_RC4_128_SHA = 6,
    TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA = 7,
    TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA = 8,
    TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA = 9,
    TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256 = 10,
    TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384 = 11,
    TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 = 12,
    SUPPORT_ALL_ABOVE = 13,
}SSL_Cipher_Suites_t;

typedef enum ssl_socket_event{
    SSL_SOCKET_RECV_EVENT = 1,
    SSL_SOCKET_CLOSE_EVENT = 2,
}SSL_Socket_Event_t;

const char ssl_ca_cert_name[] = "ca_cert.pem";
const char ssl_client_cert_name[] = "client_cert.pem";
const char ssl_client_key_name[] = "client_key.pem";

class WisLTEBG96SSL : public WisLTEBG96FILE
{
  public:
    WisLTEBG96SSL();

    ~WisLTEBG96SSL();

    WisLTEBG96SSL(Stream &atserial,  Stream &dserial);

    bool SetSSLParameters(unsigned int ssl_index, SSL_Version_t s_version, SSL_Cipher_Suites_t s_ciper, unsigned int negotiation_time);

    bool SetSSLCertificate(unsigned int ssl_index, char *ca_cert_path, char *client_cert_path, char *client_key_path, bool validity_check);

    bool InitSSL(unsigned int ssl_index, char *ca_cert, char *client_cert, char *client_key, char *err_code);

    bool OpenSSLSocket(unsigned int pdp_index, unsigned int ssl_index, unsigned int socket_index, char *ip, unsigned int port, Access_Mode_t mode);

    bool SSLSocketSendData(unsigned int socket_index, char *ssl_send_data);

    bool SSLSocketRecvData(unsigned int socket_index, unsigned int ssl_recv_len, char *ssl_recv_data);

    bool CloseSSLSocket(unsigned int socket_index);

    bool QuerySSLSocketStatus(unsigned int socket_index, char *ssl_status);

    SSL_Socket_Event_t WaitCheckSSLSocketEvent(char *event, unsigned int timeout);

  private:

};

#endif