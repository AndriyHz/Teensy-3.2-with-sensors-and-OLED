/******************************************************************************
  Template.ino
  A useful starting place when adding a TeensyView to an existing project.

  Marshall Taylor @ SparkFun Electronics, March 15, 2017
  https://github.com/sparkfun/SparkFun_TeensyView_Arduino_Library

  This example sets up the TeensyView and draws a test frame repeatedly.
  The objects in the frame were selected to give copy-paste examples for various
  common operations without a lot of chaff.  See TeensyView.h for specifics.

  Compatible with:
  Teensy LC
  Teensy 3.1
  Teensy 3.2
  Teensy 3.5
  Teensy 3.6

  Development environment specifics:
  Arduino IDE 1.6.12 w/ Teensyduino 1.31
  Arduino IDE 1.8.1 w/ Teensyduino 1.35
  TeensyView v1.0

  This code is released under the [MIT License](http://opensource.org/licenses/MIT).

  Please review the LICENSE.md file included with this example. If you have any questions
  or concerns with licensing, please contact techsupport@sparkfun.com.

  Distributed as-is; no warranty is given.
******************************************************************************/
#include <TeensyView.h>  // Include the SFE_TeensyView library
#include "SoftwareSerial.h"
#include "SparkFunBME280.h"

///////////////////////////////////
// TeensyView Object Declaration //
///////////////////////////////////
//Standard
#define PIN_RESET 14
#define PIN_DC    9
#define PIN_CS    10
#define PIN_SCK   13
#define PIN_MOSI  11

//Alternate (Audio)
//#define PIN_RESET 2
//#define PIN_DC    21
//#define PIN_CS    20
//#define PIN_SCK   14
//#define PIN_MOSI  7


TeensyView oled(PIN_RESET, PIN_DC, PIN_CS, PIN_SCK, PIN_MOSI);

// CO2 code
SoftwareSerial K_30_Serial(7,8);  //Sets up a virtual serial port
                                    //Using pin 12 for Rx and pin 13 for Tx
byte readCO2[] = {0xFE, 0X44, 0X00, 0X08, 0X02, 0X9F, 0X25};  //Command packet to read Co2 (see app note)
byte response[] = {0,0,0,0,0,0,0};  //create an array to store the response
//multiplier for value. default is 1. set to 3 for K-30 3% and 10 for K-33 ICB
int valMultiplier = 1;

// BME280 
BME280 capteur;

// O2 code
#define Version     11          // version, 1.0 or 1.1, which depands on your board you use as it is
const int pinO2     = 15;       // Connect Grove - Gas Sensor(O2) to A0
 
#if Version==11
const int AMP   = 121;
#elif Version==10
const int AMP   = 201;
#endif
 
const float K_O2    = 7.43;
 


void setup()
{
  Serial.begin(9600);
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.

//CO2 code
  //Serial.begin(9600);         //Opens the main serial port to communicate with the computer
  K_30_Serial.begin(9600);    //Opens the virtual serial port with a baud of 9600

//BME280
 //while (!Serial) //{
    // Attente de l'ouverture du port série pour Arduino LEONARDO
  //}
  //configuration du capteur
  capteur.settings.commInterface = I2C_MODE; 
  capteur.settings.I2CAddress = 0x76;
  capteur.settings.runMode = 3; 
  capteur.settings.tStandby = 0;
  capteur.settings.filter = 0;
  capteur.settings.tempOverSample = 1 ;
  capteur.settings.pressOverSample = 1;
  capteur.settings.humidOverSample = 1;
  delay(10);  // attente de la mise en route du capteur. 2 ms minimum
  // chargement de la configuration du capteur
  capteur.begin();
}

void loop()
{
 // CO2  code 
 sendRequest(readCO2);
 unsigned long valCO2 = getValue(response); 

// O2 code

   float sensorValue;
    float sensorVoltage;
    float Value_O2;
 
    sensorValue   = analogRead(15);
    sensorVoltage =((sensorValue/1024.0)*3.3)-1;
    sensorVoltage = sensorVoltage/(float)AMP*10000.0;
    Value_O2 = sensorVoltage/K_O2;
    
   
   // DISPLEY // 
  oled.clear(PAGE);  // Clear the page
  
  // CO2
  oled.setFontType(0);  // Set font to type 0
  oled.setCursor(0, 0); // move cursor
  oled.print("CO2,ppm = ");  // Write a byte out as a character
  oled.setFontType(0);  // Set font to type 0
  oled.setCursor(55, 0); // move cursor
  oled.print(valCO2);  // Write a byte out as a param
  
  // Temp
  oled.setFontType(0);  // Set font to type 0
  oled.setCursor(0, 9); // move cursor
  oled.print("Temp: = ");  // Write a byte out as a character
  oled.setFontType(0);  // Set font to type 0
  oled.setCursor(55, 9); // move cursor
  oled.print(capteur.readTempC(), 1);  // Write a byte out as a param
  
  // Press
  oled.setFontType(0);  // Set font to type 0
  oled.setCursor(0, 17); // move cursor
  oled.print("Press: = ");  // Write a byte out as a character
  oled.setFontType(0);  // Set font to type 0
  oled.setCursor(55, 17); // move cursor
  oled.print((capteur.readFloatPressure()*0.0075), 1);  // Write a byte out as a param

  // Humid
  oled.setFontType(0);  // Set font to type 0
  oled.setCursor(0, 25); // move cursor
  oled.print("Humid: = ");  // Write a byte out as a character
  oled.setFontType(0);  // Set font to type 0
  oled.setCursor(55, 25); // move cursor
  oled.print(capteur.readFloatHumidity(), 1);  // Write a byte out as a param

  // O2
  oled.setFontType(0);  // Set font to type 0
  oled.setCursor(100, 0); // move cursor
  oled.print("O2:=");  // Write a byte out as a character
  oled.setFontType(0);  // Set font to type 0
  oled.setCursor(100, 8); // move cursor
  oled.print(Value_O2, 1);  // Write a byte out as a param

  
  oled.display();  // Send the PAGE to the OLED memory
  delay(200);

 /* Serial.print("Température: ");
  Serial.print(capteur.readTempC(), 2);
  Serial.print(" °C");
  Serial.print("\t Pression: ");
  Serial.print((capteur.readFloatPressure()*0.0075), 2);
  Serial.print(" Pa");
  Serial.print("\t humidité relative : ");
  Serial.print(capteur.readFloatHumidity(), 2);
  Serial.println(" %");
  */
  delay(1000);
}
// CO2 sensor
void sendRequest(byte packet[])
{
  while(!K_30_Serial.available())  //keep sending request until we start to get a response
  {
    K_30_Serial.write(readCO2,7);
    delay(50);
  }
  
  int timeout=0;  //set a timeoute counter
  while(K_30_Serial.available() < 7 ) //Wait to get a 7 byte response
  {
    timeout++;  
    if(timeout > 10)    //if it takes to long there was probably an error
      {
        while(K_30_Serial.available())  //flush whatever we have
          K_30_Serial.read();
          
          break;                        //exit and try again
      }
      delay(50);
  }
  
  for (int i=0; i < 7; i++)
  {
    response[i] = K_30_Serial.read();
  }  
}

unsigned long getValue(byte packet[])
{
    int high = packet[3];                        //high byte for value is 4th byte in packet in the packet
    int low = packet[4];                         //low byte for value is 5th byte in the packet

  
    unsigned long val = high*256 + low;                //Combine high byte and low byte with this formula to get value
    return val* valMultiplier;
}

