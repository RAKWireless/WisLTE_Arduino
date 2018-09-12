# WisLTE_Arduino

This library is an Arduino library written based on the BG96 module, which includes functions such as TCP, HTTP, SSL, MQTT, and GNSS.

### 1. Modify the development model
This library is compatible with two development versions of WisLTE and WisCellular based on BG96 module from RAK. The user can modify the macro definition in the  `/src/WisLTEBG96Common.h`  file to adapt to the corresponding development version. `#define WISCELLULAR_BOARD`  or  `#define WISLTE_BOARD`

### 2. Turn Debug on or off
If necessary, the user can open or close the Debug serial port in the  `/src/WisLTEBG96Serial.h`  file to check the interaction between the device and the master. `#define UART_DEBUG`

### 3. Increase RX Buffer
Since RX Buffer is set to 256 in order to apply Arduino UNO, RX Buffer can be set to 1024 or higher if the user's main control space is sufficient.
The user can set the value of RX_BUFFER_LENGTH in the  `src/WisLTEBG96Serial.h` file. `#define RX_BUFFER_LENGTH  256`