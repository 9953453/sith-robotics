/*
  comms.h - Communication Protocol Implementation
  This library encapsulates the setup and usage of LoRa communication.
  Include it in your main .ino file near the top:

    #include "comms.h"

  DO NOT MODIFY UNLESS INSTRUCTED TO.

  Version: 1.0
  Author: Ryan Cather
*/

#include <RH_RF95.h>  // Include RadioHead LoRa driver

// Pin definitions for various Feather boards
// Boards with built-in radios are listed first; FeatherWing setups follow

#if defined(__AVR_ATmega32U4__)  // Feather 32u4 w/Radio
#define RFM95_CS 8
#define RFM95_INT 7
#define RFM95_RST 4

#elif defined(ADAFRUIT_FEATHER_M0) || defined(ADAFRUIT_FEATHER_M0_EXPRESS) || defined(ARDUINO_SAMD_FEATHER_M0)  // Feather M0 w/Radio
#define RFM95_CS 8
#define RFM95_INT 3
#define RFM95_RST 4

#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040_RFM)  // Feather RP2040 w/Radio
#define RFM95_CS 16
#define RFM95_INT 21
#define RFM95_RST 17

#elif defined(__AVR_ATmega328P__)  // Feather 328P w/LoRa FeatherWing
#define RFM95_CS 4
#define RFM95_INT 3
#define RFM95_RST 2

#elif defined(ESP8266)  // ESP8266 Feather w/LoRa FeatherWing
#define RFM95_CS 2
#define RFM95_INT 15
#define RFM95_RST 16

#elif defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) || defined(ARDUINO_NRF52840_FEATHER) || defined(ARDUINO_NRF52840_FEATHER_SENSE)
#define RFM95_CS 10
#define RFM95_INT 9
#define RFM95_RST 11

#elif defined(ESP32)  // ESP32 Feather w/LoRa FeatherWing
#define RFM95_CS 33
#define RFM95_INT 27
#define RFM95_RST 13
#elif defined(ARDUINO_NRF52832_FEATHER)  // nRF52832 Feather w/LoRa FeatherWing
#define RFM95_CS 11
#define RFM95_INT 31
#define RFM95_RST 7

#endif

/*
  Additional example configurations:

  // Feather 32u4:
  #define RFM95_CS   8
  #define RFM95_RST  4
  #define RFM95_INT  7

  // Feather M0:
  #define RFM95_CS   8
  #define RFM95_RST  4
  #define RFM95_INT  3
  // Arduino shield:
  #define RFM95_CS  10
  #define RFM95_RST  9
  #define RFM95_INT  7
  // Feather 32u4 w/FeatherWing:
  #define RFM95_RST 11  // "A"
  #define RFM95_CS  10  // "B"
  #define RFM95_INT  2  // "SDA"

  // Feather M0 w/FeatherWing:
  #define RFM95_RST 11  // "A"
  #define RFM95_CS  10  // "B"
  #define RFM95_INT  6  // "D"
*/

// LoRa frequency (must match receiver)
#define RF95_FREQ 915.0
// Create a singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Initializes LoRa-related pins
void initialiseLoraPins() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
}

// Performs a hardware reset of the LoRa radio
void resetRadio() {
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
}

// Initializes the LoRa radio module
void initialiseRadio() {
  while (!rf95.init()) {
    if (DEBUG) {
      Serial.println("LoRa radio init failed");
      Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    }
    while (1)
      ;  // Halt execution
  }
  if (DEBUG) {
    Serial.println("LoRa radio init OK!");
  }
}

// Sets the operating frequency of the LoRa radio
void setRadioFrequency() {
  if (!rf95.setFrequency(RF95_FREQ)) {
    if (DEBUG) {
      Serial.println("setFrequency failed");
    }
    while (1)
      ;  // Halt execution
  }
  if (DEBUG) {
    Serial.print("Set Freq to: ");
    Serial.println(RF95_FREQ);
  }
}

// Sets the transmission power level
void setRadioPower() {
  rf95.setTxPower(5, false);  // Power level: 5 dBm, use PA_BOOST pin
}

// --- MODIFIED FUNCTION PROTOTYPE ---
// Transmits a packet via LoRa
// Call this function from your main code, e.g. transmitData("test", ROVER_ID);
void transmitData(const char* radioPacket, const char* roverID);

// // Transmits a packet via LoRa
// // Call this function from your main code, e.g. transmitData("test");
// void transmitData(const char* radioPacket) {
//   digitalWrite(LED_BUILTIN, HIGH);  // Turn on LED to indicate transmission
//   static int16_t packetnum = 0;     // Optional: track packet number

//   const char* roverID = "1";
//   char packetToTx[strlen(roverID) + strlen(radioPacket) + 2];
//   strcpy(packetToTx, roverID);
//   strcat(packetToTx, ",");
//   strcat(packetToTx, radioPacket);
//   if (DEBUG) {
//     Serial.println(packetToTx);
//   }

//   rf95.send((uint8_t*)packetToTx, strlen(packetToTx) + 1);  // Send packet
//   if (DEBUG) {
//     Serial.println("Waiting for packet to complete...");
//   }
//   delay(10);
//   //rf95.waitPacketSent();  // Wait until packet is sent

//   digitalWrite(LED_BUILTIN, LOW);  // Turn off LED
// }

// Waits for a reply from the receiver
String waitForReply() {
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];  // Buffer to hold incoming message
  uint8_t len = sizeof(buf);             // Length of buffer
  if (DEBUG) {
    Serial.println("Waiting for reply...");
  }
  if (rf95.waitAvailableTimeout(100)) {  // Wait up to 1 second
    if (rf95.recv(buf, &len)) {
      if (DEBUG) {
        Serial.print("Got reply: ");

        Serial.println((char*)buf);
      }
      //Serial.print("RSSI: ");
      //Serial.println(rf95.lastRssi(), DEC);  // Print signal strength
      String str = (char*)buf;
      return str;
    } else {
      if (DEBUG) {
        Serial.println("Receive failed");
      }
      return "failed";
    }
  } else {
    if (DEBUG) {
      Serial.println("No reply, is there a listener around?");
    }
    return "No Reply";
  }
}

// Transmits a packet via LoRa
// Call this function from your main code, e.g. transmitData("test", ROVER_ID);
void transmitData(const char* radioPacket, const char* roverID) { // Added 'roverID' parameter
  digitalWrite(LED_BUILTIN, HIGH);  // Turn on LED to indicate transmission
  static int16_t packetnum = 0;     // Optional: track packet number

  // Removed 'const char* roverID = "1";'
  char packetToTx[strlen(roverID) + strlen(radioPacket) + 2];
  strcpy(packetToTx, roverID);
  strcat(packetToTx, ",");
  strcat(packetToTx, radioPacket);
  if (DEBUG) {
    Serial.println(packetToTx);
  }

  rf95.send((uint8_t*)packetToTx, strlen(packetToTx) + 1);  // Send packet
  if (DEBUG) {
    Serial.println("Waiting for packet to complete...");
  }
  delay(10);
  //rf95.waitPacketSent();  // Wait until packet is sent

  digitalWrite(LED_BUILTIN, LOW);  // Turn off LED
}
  // This function is an alternative that is meant to be continously run, this is for situations such as pinging devices without holding up the whole rover
  // it only has a timeout of 10 milliseconds to be run fast
String waitForReplyShort() {
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];  // Buffer to hold incoming message
  uint8_t len = sizeof(buf);             // Length of buffer
  if (DEBUG) {
    Serial.println("Waiting for reply...");
  }
  if (rf95.waitAvailableTimeout(10)) {  // Wait up to 1 second
    if (rf95.recv(buf, &len)) {
      if (DEBUG) {
        Serial.print("Got reply: ");

        Serial.println((char*)buf);
      }
      //Serial.print("RSSI: ");
      //Serial.println(rf95.lastRssi(), DEC);  // Print signal strength
      String str = (char*)buf;
      return str;
    } else {
      if (DEBUG) {
        Serial.println("Receive failed");
      }
      return "failed";
    }
  } else {
    if (DEBUG) {
      Serial.println("No reply, is there a listener around?");
    }
    return "No Reply";
  }
}