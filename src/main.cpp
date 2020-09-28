#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <wallet.h>

#include "Adafruit_Thermal.h"
#include "Adafruit_Keypad.h"
#include <LiquidCrystal.h>

//wifi
const char *ssid = SSID;
const char *password = PASSWORD; 
const char *host = "192.168.178.55";           //Server der die werte empfangen soll
                                               //(er sollte feste IP haben)
const char *url = "/eggs";
const char *serverName = "http://192.168.178.55:8000/eggs"; //ist doppelt und wird angepasst
String jsonPayload = "";
String jsonGet = "";
const int port = 8000;
WiFiClient client;

//Keypad
const byte ROWS = 4; // rows
const byte COLS = 4; // columns
//define the symbols on the buttons of the keypads
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {23, 22, 3, 21}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {19, 18, 5, 17}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

////Printer
#include "SoftwareSerial.h"
#define TX_PIN 13//10 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 4 //11 // Arduino receive   GREEN WIRE   labeled TX on printer

SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor

bool enableInput = false;
String input = "";

//////////Display/////
const int rs = 14, en = 27, d4 = 26, d5 = 25, d6 = 33, d7 = 32; ////32
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setDisplayMenu()
{
  lcd.noAutoscroll();
  lcd.noBlink();
  lcd.clear();

  lcd.print("A) Eier sammeln");
  lcd.setCursor(0, 1);
  lcd.print("D) Liste drucken");
}

void setup()
{
  Serial.begin(115200);
  ////////////wifi/////////////////
  //wifi
  Serial.print("Verbinde mich mit Netz: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  // WiFi.scanNetworks();
  // WiFi.begin();
  // WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Verbindung aufgebaut");
  Serial.print("Eigene IP des ESP-Modul: ");
  Serial.println(WiFi.localIP());

 
  ///////////end wifi

  ///////////Printer////////////////////
  // NOTE: SOME PRINTERS NEED 9600 BAUD instead of 19200, check test page.
  mySerial.begin(19200); // Initialize SoftwareSerial
  printer.begin();       // Init printer (same regardless of serial type)
  // printer.println(F("Das ist der Erste echte Test, mit mehr Text als in eine Zeile passt"));
  // printer.sleep();      // Tell printer to sleep
  ////////End Printer/////////////////////

  //////////// Keypad/////////////////////

  customKeypad.begin();

  /////////////End Keypad/////////////////

  ///////////////Display///////////////
  // // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  setDisplayMenu();
 

  //////////////End Diaplay////////////
}

//methods///////
String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void getRequest(const char *url, const char *host)
{
  if(WiFi.status()== WL_CONNECTED){
              
      jsonGet = httpGETRequest(serverName);
      // Serial.println(jsonGet);
      JSONVar myObject = JSON.parse(jsonGet);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(myObject);
      int sum = 0;
      
      
    
      // // myObject.keys() can be used to get an array of all the keys in the object
      // JSONVar keys = myObject.keys();
    
      // for (int i = 0; i < keys.length(); i++) {
      //   JSONVar value = myObject[keys[i]];
      //   Serial.print(keys[i]);
      //   Serial.print(" = ");
      //   Serial.println(value);
      //   sensorReadingsArr[i] = double(value);
      // }
      // Serial.print("1 = ");
      // Serial.println(sensorReadingsArr[0]);
      // Serial.print("2 = ");
      // Serial.println(sensorReadingsArr[1]);
      // Serial.print("3 = ");
      // Serial.println(sensorReadingsArr[2]);
    }
    else {
      Serial.println("WiFi Disconnected");
    }



}

//Make a post request 
void postRequest(const char *url, const char *host, String data)
{

  if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);

      // Specify content-type header
      // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // // Data to send with HTTP POST
      // String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";           
      // // Send HTTP POST request
      // int httpResponseCode = http.POST(httpRequestData);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      // int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");
      int httpResponseCode = http.POST("{\"amount\":" + data +"}");

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }


}


//prints given Text on Printer
void printTextonPrinter(String text){
  Serial.println("vor drucker");
   printer.println(F("Das ist der Erste echte Test, mit mehr Text als in eine Zeile passt"));
//   printer.justify('C');
//   printer.boldOn();
//   printer.println(F("BARCODE EXAMPLES\n"));
//   printer.boldOff();
//   printer.justify('L');

//   // There seems to be some conflict between datasheet descriptions
//   // of barcode formats and reality.  Try Wikipedia and/or:
//   // http://www.barcodeisland.com/symbolgy.phtml

//   // Also note that strings passed to printBarcode() are always normal
//   // RAM-resident strings; PROGMEM strings (e.g. F("123")) are NOT used.

//   // UPC-A: 12 digits
//   printer.print(F("UPC-A:"));
//   printer.printBarcode("123456789012", UPC_A);

//   // UPC-E: 6 digits ???
// /* Commented out because I can't get this one working yet
//   printer.print(F("UPC-E:"));
//   printer.printBarcode("123456", UPC_E);
// */

//   // EAN-13: 13 digits (same as JAN-13)
//   printer.print(F("EAN-13:"));
//   printer.printBarcode("1234567890123", EAN13);

//   // EAN-8: 8 digits (same as JAN-8)
//   printer.print(F("EAN-8:"));
//   printer.printBarcode("12345678", EAN8);

//   // CODE 39: variable length w/checksum?, 0-9,A-Z,space,$%+-./:
//   printer.print(F("CODE 39:"));
//   printer.printBarcode("ADAFRUT", CODE39);

//   // ITF: 2-254 digits (# digits always multiple of 2)
//   printer.print(F("ITF:"));
//   printer.printBarcode("1234567890", ITF);

//   // CODABAR: variable length 0-9,A-D,%+-./:
//   printer.print(F("CODABAR:"));
//   printer.printBarcode("1234567890", CODABAR);

//   // CODE 93: compressed version of Code 39?
//   printer.print(F("CODE 93:"));
//   printer.printBarcode("ADAFRUIT", CODE93);

//   // CODE 128: 2-255 characters (ASCII 0-127)
//   printer.print(F("CODE128:"));
//   printer.printBarcode("Adafruit", CODE128);

//   printer.feed(2);
//   printer.setDefault(); // Restore printer to defaults
  Serial.println("nach drucker");
}

void loop()
{
  ////Keypad
  customKeypad.tick();

  while (customKeypad.available())
  {
    keypadEvent e = customKeypad.read();

    if (e.bit.EVENT == KEY_JUST_PRESSED)
    {
      switch ((char)e.bit.KEY)
      {
      case 'A': // enable input
        if (!enableInput)
        {
          enableInput = true;
          //set Display text
          lcd.clear();
          lcd.print("Anzahl:      (B)");
          lcd.setCursor(0, 1);
          // lcd.autoscroll();
          lcd.blink();
        }
        break;
      case 'B': // send input
                    
        if (enableInput)
        {
          Serial.println(input);
          enableInput = false;

          postRequest(url, host, input);

          lcd.clear();
          lcd.print(input + " Eier");
          lcd.setCursor(0, 1);
          lcd.print("Gespeichert!");
          delay(2000);
          setDisplayMenu();
          input = "";
        }
        break;
      case 'C': //correct input
        if (enableInput)
        {
          if (!input.isEmpty())
            input.remove(input.length() - 1);
          // Serial.println("removed");
          lcd.clear();
          lcd.print("Anzahl:      (B)");
          lcd.setCursor(0, 1);
          lcd.print(input);
        }
        break;
      case 'D':
        // Print Customers of the Day
        // getRequest(url, host);
        printTextonPrinter("test");

        break;

      default:
        if (enableInput)
          input += (char)e.bit.KEY;
        lcd.setCursor(0, 1);
        lcd.print(input);
        break;
      }
      // input = (char)e.bit.KEY;
      // Serial.println(input);
    }
    // else if(e.bit.EVENT == KEY_JUST_RELEASED) Serial.println(" released");
  }

  delay(10);

}