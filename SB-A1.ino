// ---------------------------------------------------------------------------------- //
// Apple 1 Mini firmware for Arduino                                                  //
//                                                                                    //
// Version 1.1                                                                        //
// By Ruud van Falier                                                                 //
// Printer support by Simon Boak                                                      //
//                                                                                    //
// Enables serial communication between an Arduino and the Apple 1 PIA.               //
// Also implements a blinking cursor and green text using VT100 escape codes          //
// (VT100 codes only tested with PuTTY client!)                                       //
//                                                                                    //
// Heavily based on Propeller source code from Briel Computers' Apple 1 replica.      //
// ---------------------------------------------------------------------------------- //

#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#include <SPI.h>
#include <SD.h>

#define TX_PIN 10 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN A7 // Arduino receive   GREEN WIRE   labeled TX on printer (not actually connected)

SoftwareSerial printerSerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&printerSerial);     // Pass addr to printer constructor

// Port definitions (Arduino pins connected to the PIA)
#define PIN_DA  A0
#define PIN_RDA A1
#define PIN_PB0 2
#define PIN_PB1 3
#define PIN_PB2 9 // (was 4)
#define PIN_PB3 A5
#define PIN_PB4 A4
#define PIN_PB5 A3
#define PIN_PB6 A2
#define PIN_STROBE 8 // (was 12)

#define DELAY_VIDEO 1
#define DELAY_ASCII 1


#define SR_LATCH_PIN 6
#define SR_CLOCK_PIN 7
#define SR_DATA_PIN 5

#define SD_CS_PIN 4


/*
 *  SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
 */



// Global variables
uint8_t video_data = 0;
uint8_t video_data_pins[] = { PIN_PB0, PIN_PB1, PIN_PB2, PIN_PB3, PIN_PB4, PIN_PB5, PIN_PB6 };

String storage_command = "";


/*
enum video_direction {
  VIDEO_TERMINAL,
  STORAGE_CONTROLLER,
  ESCAPE_DECODER
};
video_direction direct_video_to = VIDEO_TERMINAL;
*/

File root;

bool expecting_storage_command = false;

enum storage_mode {
  RECEIVING_FILENAME
};
storage_mode storage_mode = RECEIVING_FILENAME;


bool is_loading_file = false;


bool is_cold_boot = true; // stays true until we receive the first "/" from the Apple to know that we've reset to the monitor




// Function headers
//void load_mon_file(String file_name);
void send_to_storage_controller(int video_data);


/*
 * Program initialization
 */
void setup() {
  // Setup video data pins (output from the PIA)
  pinMode(PIN_RDA, OUTPUT);
  pinMode(PIN_DA, INPUT);
  pinMode(PIN_PB0, INPUT);
  pinMode(PIN_PB1, INPUT);
  pinMode(PIN_PB2, INPUT);
  pinMode(PIN_PB3, INPUT);
  pinMode(PIN_PB4, INPUT);
  pinMode(PIN_PB5, INPUT);
  pinMode(PIN_PB6, INPUT);

  // Setup ASCII data pins (input for the PIA)
  pinMode(PIN_STROBE, OUTPUT);

  pinMode(SR_LATCH_PIN, OUTPUT);
  pinMode(SR_CLOCK_PIN, OUTPUT);
  pinMode(SR_DATA_PIN, OUTPUT);

  Serial.begin(9600);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}
 
  Serial.println();
  Serial.println("SB-A1");
  Serial.println("SB ELECTRONICS, 2021");
  Serial.println("RUUD VAN FALIER, 2017");
  Serial.println();

  // Init printer
  printerSerial.begin(19200);
  printer.begin();

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("NO SD CARD");
  } else {
    Serial.println("SD CARD FOUND");
    Serial.println("PRESS RESET TO BOOT");

    root = SD.open("/");
  }

  

}

/*
 * Program loop
 */
void loop() {
  process_video_data();
  process_keyboard_data(); 
}
