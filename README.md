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
Printer:
Printer RX: D1
Printer TX: D2
Keypad:
rowPins[ROWS] = {16, 0, 2, 14};
colPins[COLS] = {12, 13, 15, 3};