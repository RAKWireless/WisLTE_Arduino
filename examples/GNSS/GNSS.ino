
#include "WisLTEBG96GNSS.h"
#include "SoftwareSerial.h"

#define DSerial Serial1
#define ATSerial Serial
// #define AT_TX_PIN  11
// #define AT_RX_PIN  10
// SoftwareSerial  DSerial(AT_RX_PIN, AT_TX_PIN);

GNSS_Work_Mode_t mode = STAND_ALONE;

WisLTEBG96GNSS WisLTE(ATSerial, DSerial);

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

    const char inf[64];
    if(WisLTE.GetDevInformation(inf)){
        DSerial.println(inf);
    }

    while (!WisLTE.TurnOnGNSS(mode, WRITE_MODE)){
        DSerial.println("\r\nOpen the GNSS Function Fali!");
        if(WisLTE.TurnOnGNSS(mode, READ_MODE)){
            DSerial.println("\r\nThe GNSS Function is Opened!");
            WisLTE.TurnOffGNSS();
        }
    }
    DSerial.println("\r\nOpen the GNSS Function Success!");
}

void loop(){
    const char gnss_posi[128];
    while (!WisLTE.GetGNSSPositionInformation(gnss_posi)){
        DSerial.println("\r\nGet the GNSS Position Fail!");
        int e_code;
        if (WisLTE.returnErrorCode(e_code)){
            DSerial.print("\r\nERROR CODE: ");
            DSerial.println(e_code);
            DSerial.println("Please check the documentation for error details.");
        }
        delay(5000);
    }
    DSerial.println("\r\nGet the GNSS Position Success!");
    DSerial.println(gnss_posi);
}