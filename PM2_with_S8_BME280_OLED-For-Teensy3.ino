#include <TeensyView.h>  // Include the SFE_TeensyView library
// BME280
#include "SparkFunBME280.h"
#include "Wire.h"
#include "SPI.h"
///////////////////////////////////
// TeensyView Object Declaration //
///////////////////////////////////
//Standard
#define PIN_RESET 14
#define PIN_DC    9
#define PIN_CS    10
#define PIN_SCK   13
#define PIN_MOSI  11
TeensyView oled(PIN_RESET, PIN_DC, PIN_CS, PIN_SCK, PIN_MOSI);

// Code to use SoftwareSerial
#include "SoftwareSerial.h"
// CO2 code
SoftwareSerial K_30_Serial(0,1);  //Sets up a virtual serial port
                                    //Using pin 12 for Rx and pin 13 for Tx
byte readCO2[] = {0xFE, 0X44, 0X00, 0X08, 0X02, 0X9F, 0X25};  //Command packet to read Co2 (see app note)
byte response[] = {0,0,0,0,0,0,0};  //create an array to store the response
//multiplier for value. default is 1. set to 3 for K-30 3% and 10 for K-33 ICB
int valMultiplier = 1;

// BME280 
BME280 capteur;

//Read and print serial data from Plantower PMS3003

uint16_t TPM01Value = 0;        //define PM1.0 variable
uint16_t TPM2_5Value = 0;       //define PM2.5 variable
uint16_t TPM10Value = 0;       //define PM10 variable
uint16_t PM01Value = 0;        //define PM1.0 variable
uint16_t PM2_5Value = 0;       //define PM2.5 variable
uint16_t PM10Value = 0;       //define PM10 variable

uint8_t receiveDat[24];       //define array to receive data from PMS3003
uint8_t testvalue;
int cnt = 0;
int passes = 1;

void setup() {
  Serial.begin(9600);   //set the serial's Baudrate of the air detector module
  Serial3.begin(9600);
///////////////////////////////////
// TeensyView OLED //
///////////////////////////////////
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.

//CO2 code
  //Serial.begin(9600);         //Opens the main serial port to communicate with the computer
  K_30_Serial.begin(9600);    //Opens the virtual serial port with a baud of 9600

  //BME280
 //while (!Serial) {
    // Attente de l'ouverture du port série pour Arduino LEONARDO
 // }
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
      // DISPLEY // 
  oled.clear(PAGE);  // Clear the page
  
// Plantower
  //  if (Serial1.available()>0) {
  //    testvalue=Serial1.read();
  if (Serial3.available() > 0) {
    testvalue = Serial3.read();
    if (testvalue == 66 && !(cnt == 0)) {
      cnt = 0;
    }
    //    if (testvalue == 66 && cnt)
    cnt = cnt + 1;

//            Serial.print(testvalue, HEX);
//            Serial.print(" ");
    receiveDat[cnt] = testvalue;   //receive 1 of 24 bytes from air detector module
    if(receiveDat[1] != 66 && receiveDat[2] != 77){
      cnt = 0;
    }
    if (cnt == 24)
    {
      cnt = 0;
      //Serial.println("");
      TPM01Value = receiveDat[5] * 256 + receiveDat[6]; //calculate "TSI" PM1.0 concentration
      TPM2_5Value = receiveDat[7] * 256 + receiveDat[8]; //calculate "TSI" PM2.5 concentration
      TPM10Value = receiveDat[9] * 256 + receiveDat[10]; //calculate "TSI" PM10 concentration
     // Serial.print("{TSI 01 = ");
     // Serial.print(TPM01Value);
     // Serial.print(", TSI 2.5 = ");
     // Serial.print(TPM2_5Value);
     // Serial.print(", TSI 10 = ");
     // Serial.print(TPM10Value);
      PM01Value = receiveDat[11] * 256 + receiveDat[12]; //calculate PM1.0 concentration
      PM2_5Value = receiveDat[13] * 256 + receiveDat[14]; //calculate PM2.5 concentration
      PM10Value = receiveDat[15] * 256 + receiveDat[16]; //calculate PM10 concentration
 // CO2  code 
 sendRequest(readCO2);
 unsigned long valCO2 = getValue(response); 

      // DISPLEY // 
  
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
  
  // PM2.5
  oled.setFontType(0);  // Set font to type 0
  oled.setCursor(90, 0); // move cursor
  oled.print("PM2.5:");  // Write a byte out as a character
  oled.setFontType(0);  // Set font to type 0
  oled.setCursor(90, 8); // move cursor
  oled.print(PM2_5Value, 1);  // Write a byte out as a param
  
  // PM10
  oled.setFontType(0);  // Set font to type 0
  oled.setCursor(90, 17); // move cursor
  oled.print("PM10:");  // Write a byte out as a character
  oled.setFontType(0);  // Set font to type 0
  oled.setCursor(90, 25); // move cursor
  oled.print(PM10Value, 1);  // Write a byte out as a param
  oled.display();  // Send the PAGE to the OLED memory
  
////////////////////////////////////////////////
      Serial.print("PM01 Value = ");
      Serial.print(PM01Value);
      Serial.println("  ug/m3");
      Serial.print("PM2.5 Value = ");
      Serial.print(PM2_5Value);
      Serial.println("  ug/m3");  
      Serial.print("PM10 Value = ");
      Serial.print(PM10Value);
      Serial.println("  ug/m3");
      Serial.println();   
      delay(250); 
      Serial.print("Température: ");
      Serial.print(capteur.readTempC(), 2);
      Serial.print("°C");
      Serial.print("\t Pression: ");
      Serial.print((capteur.readFloatPressure()*0.0075), 2);
      Serial.print(" Pa");
      Serial.print("\t Humidité relative : ");
      Serial.print(capteur.readFloatHumidity(), 2);
      Serial.print(" %");
      Serial.print("\t Co2 ppm = ");
      Serial.println(valCO2);
      Serial.println("\t =============== ");
      delay(250);
    }
  }
 
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

