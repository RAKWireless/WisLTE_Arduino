/*
 * A library for RAK WisLTE BG96 Development board
 * This file is about the BG96 TCP(IP) function
 * 
 * Copyright (c) 2018 RAKWireless technology inc.
 * @Author       : Chace
 * @Create time  : 08/19/2018
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

#ifndef _WISLTEBG96TCPIP_H_
#define _WISLTEBG96TCPIP_H_

#include "WisLTEBG96Common.h"

typedef enum protocol_type{
    IPV4 = 1,
    IPV4V6 = 2,
}Protocol_Type_t;

typedef enum authentication_methods{
    NONE = 0,
    PAP = 1,
    CHAP = 2,
    PAP_OR_CHAP = 3,
}Authentication_Methods_t;

typedef enum socket_type{
    TCP_CLIENT = 0,
    TCP_SEVER = 1,
    UDP_CLIENT = 2,
    UDP_SEVER = 3,
}Socket_Type_t;

typedef enum access_mode{
    BUFFER_MODE = 0,
    DIRECT_PUSH_MODE = 1,
    TRANSPARENT_MODE = 2,
}Access_Mode_t;

typedef enum socket_event{
    SOCKET_RECV_DATA_EVENT = 1,
    SOCKET_CLOSE_EVENT = 2,
    SOCKET_PDP_DEACTIVATION_EVENT = 3,
    SOCKET_INCOMING_CONNECTION_EVENT = 4,
    SOCKET_CONNECTION_FULL_EVENT = 5,
}Socket_Event_t;

class WisLTEBG96TCPIP : public WisLTEBG96Common
{
  public:
    WisLTEBG96TCPIP();

    ~WisLTEBG96TCPIP();

    WisLTEBG96TCPIP(Stream &atserial, Stream &dserial); 

    bool SetDevAPNParameters(unsigned int pdp_index, Protocol_Type_t type, char *apn, char *usr, char *pwd, Authentication_Methods_t met);

    Cmd_Response_t ActivateDevAPN(unsigned int pdp_index);

    bool DeactivateDevAPN(unsigned int pdp_index);

    bool GetDevAPNIPAddress(unsigned int pdp_index, char *ip);

    bool InitAPN(unsigned int pdp_index, char *apn, char *usr, char *pwd, char *err_code);

    bool OpenSocketService(unsigned int pdp_index, unsigned int socket_index, Socket_Type_t socket, char *ip, unsigned int port, unsigned int local_port, Access_Mode_t mode);

    bool CloseSocketService(unsigned int socket_index);

    bool SocketSendData(unsigned int socket_index, Socket_Type_t socket, char *data_buf, char *ip, unsigned int port);

    bool SocketRecvData(unsigned int socket_index, unsigned int recv_len, Socket_Type_t socket, char *recv_buf);

    bool SocketSendHEXData(unsigned int socket_index, char *hex_buf);

    bool SwitchAccessModes(unsigned int socket_index, Access_Mode_t mode);

    bool DevPingFunction(unsigned int socket_index, char *host);

    bool DevNTPFunction(unsigned int socket_index, char *ntp_ip, unsigned int port, char *time);

    bool ConfigDNSServer(unsigned int socket_index, char *primary_addr, char *secondary_addr);

    bool DevDNSFunction(unsigned int socket_index, char *domain_name, char *ip);

    bool QueryLastErrorCode(char *err_code);

    Socket_Event_t WaitCheckSocketEvent(char *event, unsigned int timeout);

  private:
  
};

#endif