/*
TFM WeatherStation
Peter Hein Nov 2016

version 0.3
    change log - removing delay and using millis
               - need to rethink how and cleanup
               - clean up formatting
version 0.4
    change log - User Interface work on LCD
               - add photoresistor for LCD on/off
               - add photoresistor on to PinA0
               - move LCD from power to Pin7
version 0.5
    change log - remove light sensor
               - add BMP280 hardware + framework
               - add DS3231 RTC hardware + framework
               - cleanup

LCD pint outs LCD 5110
1 VCC - Supply range is between 2.7V and 3.3V
2 GND - Ground    
3 SCE - Screen
4 RST - Reset
5 D/C - Mode
6 DN(MOSI)
7 SCLK  
8 LED

LCD5110 myGLCD(8,9,10,11,12)

I2C - both in-line using SDA / SCL pins A4 and 5 on UNO
      BMP address 0x76
      DS3231 address 0x68

using pin13 LED right now for timing until RTC arrives
*/

#include <LCD5110_Basic.h>
//#include <LCD5110_Graph.h>
#include <Wire.h>
#include <BME280.h>
#include "DHT.h"
#include "RTClib.h"

// Working with the RTC
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Temp / press / alt sensor
BME280 bme;

//Define the DHT22 sensor
#define DHTPIN 2
#define DHTTYPE DHT22

// I2C address for DS3231 RTC
#define RTC_i2c_AD 0x68

// I2C address for BMP280
#define BMP280 0x76

//The Nokia 5110 LCD
LCD5110 myGLCD(8,9,10,11,12);
    extern uint8_t SmallFont[];
    extern uint8_t MediumNumbers[];
    extern uint8_t BigNumbers[];

// Setup DHT
DHT dht(DHTPIN, DHTTYPE);


// set LED pin for system running
// and timing
const int bckLghtPin =  7;
int bckLghtState = LOW;

int getTime = 0;

          

void setup() {
   // setup DS3231 RTC
   // This line sets the RTC with an explicit date & time, for example to set
   // 2016 November 18 11:39:0 you would call:
   //rtc.adjust(DateTime(2016, 11, 18, 1, 24, 0));

   // set the digital pin as output:
   pinMode(bckLghtState, OUTPUT);
   pinMode(bckLghtPin, OUTPUT);
   // Setup the LCD
   myGLCD.InitLCD();
   myGLCD.setFont(SmallFont);
   myGLCD.setContrast(60);

   Serial.begin(9600);

   dht.begin();
   rtc.begin();
   bme.begin();
}

void loop() {
  // Turn on the display
   analogWrite(bckLghtPin, 255);
   // Lets deal with the time. This part is here for debugging
   // and a place to start with DS3231
   DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
  delay(500);

    // Set temperature and humidity
    // for the DHT22
   float hum22 = dht.readHumidity();
   float temp22 = dht.readTemperature();
//   float getTime = rtc.now();

   // Check to see if the DHT is present
   // if not - error
   if (isnan(hum22) || isnan(temp22)) {
      myGLCD.clrScr();
      myGLCD.print("Device Not Found",15,1);
      Serial.print("Device Not Found");
      delay(2000);

   }

  else {
      // Time to print some data
      // Need to work on the formatting.
      // Goal: to put all the reading on one screen
     // Goal: Historical data on graph
      myGLCD.clrScr();
      //myGLCD.setFont(SmallFont);
      myGLCD.print("TFM Weather",0,0);
      // Time up on the screen, It aint purdy
      // but working
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.println();
      myGLCD.print("Temp=",0,20);
      myGLCD.printNumI(temp22,35,20);
      //Serial.print(temp);
      myGLCD.print("~C",50,20);
      myGLCD.print("Hum.=",0,31);
      myGLCD.printNumI(hum22,35,31);
      //Serial.print(hum);
      myGLCD.print("%",50,31);
      delay(5000);
      myGLCD.clrScr();
      myGLCD.print(getTime, 10,30);
      delay(5000);

}

}



