#include <Arduino.h>
// Feather9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_RX


// Set to true to enable Serial comms.
// set to false to stop all serial comms.


#define DEBUG false //This makes the device not work until you open the serial monitor, only use when you need to debug
//otherwise turn off for devices not connected to a computer


// --- NEW CONSTANT DEFINITION ---
const char* ROVER_ID = "1"; // Define the constant ID here

// Board Libraries:
// https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
// https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "Adafruit_miniTFTWing.h"



Adafruit_miniTFTWing ss;
#define TFT_RST -1  // we use the seesaw for resetting to save a pin
#define TFT_CS 5
#define TFT_DC 6
Adafruit_ST7735 tft_7735 = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Adafruit_ST77xx *tft = NULL;
unsigned long startTime;
bool hasSentStop = false;

#include "comms.h"


void initialiseTFT() {
  if (!ss.begin()) {
    if (DEBUG) {
      Serial.println("seesaw couldn't be found!");
    }
    while (1)
      ;
  }

  ss.tftReset();                          // reset the display
  ss.setBacklight(TFTWING_BACKLIGHT_ON);  // turn off the backlight
  tft_7735.initR(INITR_MINI160x80);       // initialize a ST7735S chip, mini display
  tft = &tft_7735;
  tft->setRotation(1);
    tft->fillScreen(ST77XX_BLACK);

  if (DEBUG) {
    Serial.println("TFT initialized");
  }
}


// Initialises the Serial Monitor
// Waits for Serial to be ready before continuing
void initialiseSerial() {
  Serial.begin(9600);                  // Start serial at 9600 baud
  while (!Serial) delay(1);            // Wait for Serial to be ready
  delay(100);                          // Short delay for stability
  Serial.println("Feather LoRa TX!");  // Print startup message
}

// Sends a message only when the Feather's BOOT button is pressed
void debugTransmissionButton() {
  pinMode(7, INPUT);               // Set pin 7 as input (BOOT button)
  if (!digitalRead(7)) {           // Check if button is pressed
    transmitData("Button Press", ROVER_ID);  // Send message
    waitForReply();                // Wait for response
  }
}

// Sends a test message and waits for a reply
// Useful for basic transmission debugging
void debugTransmissionSimple() {
  transmitData("Ryan", ROVER_ID);  // Send test message
  waitForReply();        // Wait for response
}

void cycleBasicCommands() {
  transmitData("test", ROVER_ID);
  waitForReply();
  transmitData("forward", ROVER_ID);
  waitForReply();
  transmitData("backward", ROVER_ID);
  waitForReply();
  transmitData("left", ROVER_ID);
  waitForReply();
  transmitData("right", ROVER_ID);
  waitForReply();
  transmitData("stop", ROVER_ID);
  waitForReply();
  transmitData("start", ROVER_ID);
  waitForReply();
}

// This function will run super fast, so that it can stop as soon as you stop pressing any buttons
// The movement function will run slower as to not spam the lora network
void transmitStopCommand(){
  uint32_t buttons = ss.readButtons();
  
  if (buttons == 3740 & hasSentStop == false){
    for (int i = 0; i <= 2; i++){ 
    transmitData("stop", ROVER_ID);
  }
    hasSentStop = true;
    return;
  }

}

void transmitButtonCommands() {
  uint32_t buttons = ss.readButtons();

  uint16_t color;


  // 3740 is the value that is printed when no buttons are pressed, the number is changed when buttons are pressed.
  // not sure exactly what the number means yet, to be investigated

  color = ST77XX_BLACK;
  if (!(buttons & TFTWING_BUTTON_LEFT)) {
    // Serial.println("LEFT");
    color = ST77XX_WHITE;
    transmitData("left", ROVER_ID);
    hasSentStop = false;
  }

  tft->fillTriangle(150, 30, 150, 50, 160, 40, color);


  color = ST77XX_BLACK;
  if (!(buttons & TFTWING_BUTTON_RIGHT)) {
    // Serial.println("RIGHT");
    color = ST77XX_WHITE;
    transmitData("right", ROVER_ID);
    hasSentStop = false;
  }

  tft->fillTriangle(120, 30, 120, 50, 110, 40, color);


  color = ST77XX_BLACK;
  if (!(buttons & TFTWING_BUTTON_DOWN)) {
    // Serial.println("DOWN");
    color = ST77XX_WHITE;
    transmitData("backward", ROVER_ID);
    hasSentStop = false;
  }

  tft->fillTriangle(125, 26, 145, 26, 135, 16, color);


  color = ST77XX_BLACK;
  if (!(buttons & TFTWING_BUTTON_UP)) {
    // Serial.println("UP");
    color = ST77XX_WHITE;
    transmitData("forward", ROVER_ID);
    hasSentStop = false;
  }

  tft->fillTriangle(125, 53, 145, 53, 135, 63, color);


  color = ST77XX_BLACK;
  if (!(buttons & TFTWING_BUTTON_A)) {
    // Serial.println("A");
    color = ST7735_GREEN;
    transmitData("beep", ROVER_ID);
    hasSentStop = false;
  }

  tft->fillCircle(30, 57, 10, color);


  color = ST77XX_BLACK;
  if (!(buttons & TFTWING_BUTTON_B)) {
    // Serial.println("B");
    color = ST77XX_YELLOW;
  }

  tft->fillCircle(30, 18, 10, color);


  color = ST77XX_BLACK;
  if (!(buttons & TFTWING_BUTTON_SELECT)) {
    // Serial.println("SELECT");
    color = ST77XX_RED;
    transmitData("beepTwice", ROVER_ID);
    hasSentStop = false;
  }

  tft->fillCircle(135, 40, 7, color);
  waitForReply();
  
}


// Setup function runs once at startup
void setup() {
  initialiseLoraPins();  // Configure LoRa module pins
  if (DEBUG) {
    initialiseSerial();
  }// Start serial communication
  resetRadio();         // Reset the LoRa radio module
  initialiseRadio();    // Initialise radio settings
  setRadioFrequency();  // Set operating frequency
  setRadioPower();      // Set transmission power
  initialiseTFT();
  pinMode(LED_BUILTIN, OUTPUT);  // Set built-in LED pin as output
}


// Main loop runs repeatedly after setup
void loop() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;
  if (elapsedTime >= 300){
    transmitButtonCommands();
    startTime = currentTime; 
  }
  transmitStopCommand();
  // Uncomment one of the following for debugging transmission

  // debugTransmissionSimple();  // Send a test message
  // debugTransmissionButton();    // Send message when button is pressed
  // cycleBasicCommands();
  delay(10);  // Wait 1 second before next loop iteration
}