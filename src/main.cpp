#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <wallet.h>

#include "Adafruit_Thermal.h"
#include "Adafruit_Keypad.h"
#include <LiquidCrystal.h>
// #include "qrprint.h" //qrcode generator for thermal printer
#include "qrcode.h"
#include "qrImage.h"

#define  GPIO_INPUT_IO_TRIGGER 5

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
// const int rs = 14, en = 27, d4 = 26, d5 = 25, d6 = 33, d7 = 32; ////
const int rs = 32, en = 33, d4 = 25, d5 = 26, d6 = 27, d7 = 14; ////

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

//rest call auf die menge der eier von heute
String getAmountofToday()
{
  if(WiFi.status()== WL_CONNECTED){
              
      char *endpoint = "http://192.168.178.55:8000/eggs/today/amount";
      jsonGet = httpGETRequest(endpoint);
      JSONVar myObject = JSON.parse(jsonGet);
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return "Parsing input failed!";
      }
      // Serial.print("JSON object = ");
      // Serial.println(myObject);
      return JSON.stringify(myObject);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
}

//rest call auf die menge der eier von gestern
String getAmountofYesterday()
{
  if(WiFi.status()== WL_CONNECTED){
              
      char *endpoint = "http://192.168.178.55:8000/eggs/yesterday/amount";
      jsonGet = httpGETRequest(endpoint);
      JSONVar myObject = JSON.parse(jsonGet);
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return "Parsing input failed!";
      }
      // Serial.print("JSON object = ");
      // Serial.println(myObject);
      return JSON.stringify(myObject);
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

      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST("{\"amount\":" + data +"}");
     
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
void printCustomerList(){

/////test customers//////+
char *customerNames[3] = {"Schmidt", "Meier", "Mueller"};
char *customerAmount[3] = {"20 Eier", "40 Eier", "20 Eier"};

String remoteSystemTime;
String eggsYesterday = getAmountofYesterday();
String eggsToday = getAmountofToday();

  //get current system time and prints it in reverse
  printer.inverseOn();
  printer.justify('C');
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];
  time (&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer,sizeof(buffer),"%d:%m:%Y %H:%M:%S",timeinfo);
  String str(buffer);
  printer.println((str));
  printer.inverseOff();


printer.justify('L');
for (int i = 0; i < 3; i++){
  printer.println(F(customerNames[i]));
  printer.println(F(customerAmount[i]));
  printer.println(F("__________________________"));
}

printer.println(("Gestern wurden insgesamt " + eggsYesterday + " Eier gelegt"));
printer.println(("Heute wurden insgesamt " + eggsToday + " Eier gelegt"));
printer.println("\n");
printer.printBitmap(150, 150, image_data_qrcode); // prints qr code 
printer.println("\n\n");

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
        // Serial.println( getAmountofToday());
        printCustomerList();

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




// ////////creates an qr code and convert it to bitmap - funktioniert, nur sehr klein (version 3)////////////////

// // The structure to manage the QR code
// QRCode qrcode;

// // default is VERSION 3: 29x29
// // Allocate a chunk of memory to store the QR code
// uint8_t qrcodeBytes[qrcode_getBufferSize(3)];

// qrcode_initText(&qrcode, qrcodeBytes, 3, ECC_LOW, "HELLO WORLD");

// // thermal printer needs multiples of 8 so make it 32 x 32
// // which is 32/8 * 32 = 128 bytes
// const int printerBuffer = 32/8 * 32;
// uint8_t printerData[printerBuffer];

// memset(printerData, 0, printerBuffer);

// for (uint8_t y = 0; y < qrcode.size; y++) {
//   for (uint8_t x = 0; x < qrcode.size; x++) {
//     if (qrcode_getModule(&qrcode, x, y)) {
//     // bit is on so set it in printerData
//     uint8_t pos = y * 4 + x / 8;  // byte position
//     uint8_t idx = 7 - x % 8;      // bit position,
//     printerData[pos] |= (1 << idx);
//     }
//   }
// }
// // printer.setSize('L');        // Set type size, accepts 'S', 'M', 'L'
// printer.printBitmap(32,32,printerData,false);
// // printer.setDefault(); // Restore printer to defaults
  
// ///////////////////////////////////////////////////////////////////