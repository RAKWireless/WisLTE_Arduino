
#include "WisLTEBG96HTTP.h"
#include "SoftwareSerial.h"

#define DSerial Serial1
#define ATSerial Serial
// #define AT_TX_PIN  11
// #define AT_RX_PIN  10
// SoftwareSerial  DSerial(AT_RX_PIN, AT_TX_PIN);

const char APN[] = "hologram";
const char http_url[] = "http://www.baidu.com";   //"http://app.tongxinmao.com:89/app/api/ip";
unsigned int comm_pdp_index = 1;    // The range is 1 ~ 16
HTTP_Body_Data_Type_t  http_type = APPLICATION_X_WWW_FORM_URLENCODED;

WisLTEBG96HTTP WisLTE(ATSerial, DSerial);

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

    while(!WisLTE.SetHTTPConfigParameters(comm_pdp_index, false, false, http_type)){
        DSerial.println("\r\nConfig the HTTP Parameter Fail!");
        int e_code;
        if (WisLTE.returnErrorCode(e_code)){
            DSerial.print("\r\nERROR CODE: ");
            DSerial.println(e_code);
            DSerial.println("Please check the documentation for error details.");
            while(1);
        }
    }
    DSerial.println("\r\nConfig the HTTP Parameter Success!");

    while(!WisLTE.HTTPURL(http_url, WRITE_MODE)){
        DSerial.println("\r\Set the HTTP URL Fail!");
        int e_code;
        if (WisLTE.returnErrorCode(e_code)){
            DSerial.print("\r\nERROR CODE: ");
            DSerial.println(e_code);
            DSerial.println("Please check the documentation for error details.");
            while(1);
        }
    }
    DSerial.println("\r\Set the HTTP URL Success!");
}

void loop(){
    const char recv_file[] = "http_read.txt";
    // char recv_data[128];
    if(!WisLTE.HTTPGET(80)){
        DSerial.println("\r\nHTTP GET Success!");
        int e_code;
        if (WisLTE.returnErrorCode(e_code)){
            DSerial.print("\r\nERROR CODE: ");
            DSerial.println(e_code);
            DSerial.println("Please check the documentation for error details.");
            while(1);
        }
    }
    DSerial.println("\r\nHTTP GET Success!");

    if(WisLTE.HTTPReadToFile(recv_file, 80)){
        DSerial.println("\r\nHTTP Read to File Success!");
    }
    // if(WisLTE.HTTPRead(recv_data, 80)){
    //     DSerial.println("\r\nHTTP Read Success!");
    //     DSerial.println("");
    //     DSerial.println(recv_data);
    //     DSerial.println("");
    // }
    while(1);
}