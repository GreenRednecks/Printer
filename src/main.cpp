#include <Arduino.h>


#include "Adafruit_Thermal.h"


#include "SoftwareSerial.h"
#define TX_PIN 5 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 4 // Arduino receive   GREEN WIRE   labeled TX on printer

SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor


void setup() {

    // NOTE: SOME PRINTERS NEED 9600 BAUD instead of 19200, check test page.
  mySerial.begin(19200);  // Initialize SoftwareSerial
  
  printer.begin();        // Init printer (same regardless of serial type)

printer.println(F("Das ist der Erste echte Test, mit mehr Text als in eine Zeile passt"));

  // printer.sleep();      // Tell printer to sleep

}

void loop() {
}