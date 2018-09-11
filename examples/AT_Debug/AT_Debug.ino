

#include "WisLTEBG96Serial.h"
#include "SoftwareSerial.h"

#define DSerial Serial1
#define ATSerial Serial
//#define AT_TX_PIN  11
//#define AT_RX_PIN  10
//SoftwareSerial  DSerial(AT_RX_PIN, AT_TX_PIN);

WisLTEBG96Serial WisLTE(ATSerial, DSerial);

void setup(){
    DSerial.begin(115200);
    while(DSerial.read() >= 0);
    DSerial.println("This is the WisLTE Debug Serial!");
    delay(1000);
    
    ATSerial.begin(115200);
}

void loop(){
    WisLTE.AT_bypass();
}