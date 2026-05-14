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
const char* ROVER_ID = "1"; // Define the constant ID here (redundant for now)

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
int selectedOption = 0;
String currentMenu = "mainMenu";

#include "comms.h"
int currentRoverID = 1; //This is a debug command; remove if its not working.
bool showID = false; // same with all these
unsigned long idDisplayStart = 0;
const int idDisplayDuration = 700; // ms (adjust to taste)

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
  tft->setRotation(3);
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
    //transmitData("Button Press", ROVER_ID);  // Send message
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
    //transmitData("stop", ROVER_ID);
    transmitData("stop", String(currentRoverID).c_str());
  }
    hasSentStop = true;
    return;
  }

}

void buttonTransmit() {
  uint32_t buttons = ss.readButtons();

  uint16_t color;


  // 3740 is the value that is printed when no buttons are pressed, the number is changed when buttons are pressed.
  // not sure exactly what the number means yet, to be investigated

  color = ST77XX_BLACK;
  if (!(buttons & TFTWING_BUTTON_LEFT)) {
    // Serial.println("LEFT");
    color = ST77XX_WHITE;
    //transmitData("left", ROVER_ID);
    transmitData("left", String(currentRoverID).c_str());
    hasSentStop = false;
  }

  tft->fillTriangle(150, 30, 150, 50, 160, 40, color);


  color = ST77XX_BLACK;
  if (!(buttons & TFTWING_BUTTON_RIGHT)) {
    // Serial.println("RIGHT");
    color = ST77XX_WHITE;
    //transmitData("right", ROVER_ID);
    transmitData("right", String(currentRoverID).c_str());
    hasSentStop = false;
  }

  tft->fillTriangle(120, 30, 120, 50, 110, 40, color);


  color = ST77XX_BLACK;
  if (!(buttons & TFTWING_BUTTON_DOWN)) {
    // Serial.println("DOWN");
    color = ST77XX_WHITE;
    //transmitData("backward", ROVER_ID);
    transmitData("backward", String(currentRoverID).c_str());
    hasSentStop = false;
  }

  tft->fillTriangle(125, 26, 145, 26, 135, 16, color);


  color = ST77XX_BLACK;
  if (!(buttons & TFTWING_BUTTON_UP)) {
    // Serial.println("UP");
    color = ST77XX_WHITE;
    //transmitData("forward", ROVER_ID);
    transmitData("forward", String(currentRoverID).c_str());
    hasSentStop = false;
  }

  tft->fillTriangle(125, 53, 145, 53, 135, 63, color);


  color = ST77XX_BLACK;
  if (!(buttons & TFTWING_BUTTON_A)) {
    // Serial.println("A");
    color = ST7735_GREEN;
    //transmitData("beep", ROVER_ID);
    transmitData("beep", String(currentRoverID).c_str());
    hasSentStop = false;
  }

  tft->fillCircle(30, 57, 10, color);


  color = ST77XX_BLACK;
  if (!(buttons & TFTWING_BUTTON_B)) {
    // Serial.println("B");
    currentMenu = "mainMenu";
  }


  color = ST77XX_BLACK;
  if (!(buttons & TFTWING_BUTTON_SELECT)) {
    // Serial.println("SELECT");
    currentRoverID++;
    if (currentRoverID > 5) currentRoverID = 1;
    color = ST77XX_RED;
    //transmitData("IDCycle", ROVER_ID);
    transmitData("IDCycle", String(currentRoverID).c_str());
    hasSentStop = false;
    showID = true;
    idDisplayStart = millis();
    delay(300);


  }

  tft->fillCircle(135, 40, 7, color);
  waitForReply();
  
}

#define ITEMS_PER_PAGE 3
#define ITEM_HEIGHT   20

// Add/remove menu items here - everything else auto-adjusts
const char* mainMenuItems[] = {
  "Driving",      // 0
  "Ping ID",   // 1
  "Receive",      // 2
  "Check ID",      // 3
  "HEEELP",       // 4
  "WOOOAH"        // 5
};
#define MAIN_MENU_COUNT (sizeof(mainMenuItems) / sizeof(mainMenuItems[0]))

// Helper: draw a single menu item at screen position
void drawMenuItem(int menuIndex, int screenRow, bool isSelected) {
  tft->setCursor(0, screenRow * ITEM_HEIGHT);
  tft->setTextSize(2);
  tft->print(isSelected ? ">" : " ");
  tft->print(mainMenuItems[menuIndex]);
}

void mainMenuLogic(){
  uint32_t buttons = ss.readButtons();
  uint16_t color;
  
  // Clear screen once per frame (reduces flicker vs. clearing on every button press)
  
  // Calculate which items to show (auto-paging)
  int currentPageStart = (selectedOption / ITEMS_PER_PAGE) * ITEMS_PER_PAGE;
  int currentPageEnd = currentPageStart + ITEMS_PER_PAGE;
  if (currentPageEnd > MAIN_MENU_COUNT) currentPageEnd = MAIN_MENU_COUNT;
  
  // Draw visible items using a loop (no more repetitive code!)
  for (int i = currentPageStart; i < currentPageEnd; i++) {
    int screenRow = i - currentPageStart; // 0, 1, or 2
    drawMenuItem(i, screenRow, (i == selectedOption));
  }

  
  // Scroll UP
  if (!(buttons & TFTWING_BUTTON_UP) && selectedOption > 0) {
    tft->fillScreen(ST77XX_BLACK);
    selectedOption--;
  }
  
  // Scroll DOWN  
  if (!(buttons & TFTWING_BUTTON_DOWN) && selectedOption < MAIN_MENU_COUNT - 1) {
    tft->fillScreen(ST77XX_BLACK);
    selectedOption++;
  }
  
  // Select option (A button)
  if (!(buttons & TFTWING_BUTTON_A)) {
    switch(selectedOption) {
      case 0: currentMenu = "driving";  tft->fillScreen(ST77XX_BLACK); break;
      case 1: currentMenu = "pingMenu"; tft->fillScreen(ST77XX_BLACK); break;              // Exit menu
      case 2: currentMenu = "receiver"; tft->fillScreen(ST77XX_BLACK); break;
      // Add more cases here for options 3, 4, 5, etc.
      // case 3: /* your code */ break;
      // case 4: /* your code */ break;
      // case 5: /* your code */ break;
      default: break; // Safety fallback
    }
  }
}


void receiverMenuLogic(){
  uint32_t buttons = ss.readButtons();
  uint16_t color;
  tft->setCursor(0, 0);
  tft->setTextSize(1);

  String reply = waitForReply();
  if(reply != "No Reply"){
    tft->fillScreen(ST77XX_BLACK);
    tft->print(reply);
    tft->println(rf95.lastRssi(), DEC);
  }
  
  if (!(buttons & TFTWING_BUTTON_B)) {
    // Serial.println("B");
    tft->fillScreen(ST77XX_BLACK);
    currentMenu = "mainMenu";
  }
}

void pingMenuLogic(){
  tft->setCursor(0, 0);
  tft->setTextSize(1);
  transmitData("Ping Devices", ROVER_ID);
  delay(10);
  String reply = waitForReply();
  if(reply != "No Reply"){
    tft->print("A Device pinged back!");
  } else{
    tft->print("No devices in range");
  }
  delay(2000);
  currentMenu = "mainMenu";
}

void drawMenu(){
  //Menu logic where code is ran depending on menu, such as running the driving function when driving the rover
  //Each menu would have its own function
  if (currentMenu == "driving"){
    buttonTransmit();
  } 
  
  if (currentMenu == "mainMenu"){
    mainMenuLogic();
  }

  if (currentMenu == "receiver"){
    receiverMenuLogic();
  }

  if (currentMenu == "pingMenu"){
    pingMenuLogic();
  }
  //This is the end of the main menu functionality
}

void handleIDDisplay() {
  if (showID) {
    // Draw small background (same size vibe as your circles)
    tft->fillRect(70, 20, 60, 80, ST77XX_BLACK);

    tft->setCursor(80, 32);
    tft->setTextColor(ST77XX_WHITE);
    tft->setTextSize(5);
    tft->print(currentRoverID);

    // Check timeout
    if (millis() - idDisplayStart > idDisplayDuration) {
      // Clear it
      tft->fillRect(120, 5, 20, 15, ST77XX_BLACK);
      showID = false;
    }
  }
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

  tft->fillScreen(ST77XX_RED);
  delay(100);
  tft->fillScreen(ST77XX_GREEN);
  delay(100);
  tft->fillScreen(ST77XX_BLUE);
  delay(100);
  tft->fillScreen(ST77XX_BLACK);
  delay(100);
  tft->setTextWrap(true);
  tft->setCursor(0, 0);
  tft->setTextColor(ST77XX_WHITE);
  tft->setTextSize(2);
  tft->println("Welcome To");
  tft->println("SITH OS");
  //SITH = Super Intelligent Telecoms Home
  tft->setTextSize(1);
  tft->println("");
  tft->println("Super Intelligent Tele Home");
  delay(2000);
  tft->fillScreen(ST77XX_BLACK);
  tft->setCursor(0, 0);
  tft->setTextSize(2);
}


// Main loop runs repeatedly after setup
void loop() {
  String shortReply;
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;
  if (currentMenu == "driving"){
    if (elapsedTime >= 300){
      buttonTransmit();
      startTime = currentTime; 
    }
    transmitStopCommand();
  }
  

  // Uncomment one of the following for debugging transmission

  // debugTransmissionSimple();  // Send a test message
  // debugTransmissionButton();    // Send message when button is pressed
  // cycleBasicCommands();
  
  shortReply = waitForReplyShort();
  if (shortReply == String(ROVER_ID) + ",Ping Devices"){
    transmitData("Controller", ROVER_ID);
  }
  drawMenu();
  delay(90);
}