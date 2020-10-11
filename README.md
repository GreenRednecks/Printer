# Printer
This is a printer to print out informations abous Eggs

## Setup
The project is setup with platform.io \n
Hardware:
- ESP 32
- Printer
Librays:
- Adafruit Thermal Printer Library
- Adafruit Keypad
- LiquidCrystal

## PinOut
Keypad:
Pins rows (first 4 left to right): 23, 22, 3, 21
Pins col: (last 4 left to right) 19, 18, 5, 17

Printer:
TX_PIN 13  //Arduino transmit  YELLOW WIRE  labeled RX on printer
RX_PIN 4   //Arduino receive   GREEN WIRE   labeled TX on printer
requrires 9v to power

Display:
rs = 14, en = 27, d4 = 26, d5 = 25, d6 = 33, d7 = 32
vss = GND, vdd = 5v, v0 = poti against ground, a = resistor to 5v, k = GND


when usb on the left, display uses pins on top and keypad on the bottom
