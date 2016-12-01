// wrightmac Labs 2016
// Peter Hein
// version 0.6

#include <LCD5110_Basic.h>
#include <Wire.h>
#include "SPI.h"
#include "DHT.h"
#include "SparkFunBME280.h"
#include <stdint.h>

// DHT22 Connection
// All pin on the sensor go from right to left looking at
// front of the sensor, side with the grill
// Connect pin 1 to +5V
// Connect pin 2 of the sensor to DHTPIN
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
// Connect pin 4 to GROUND
// define which pin
#define DHTPIN 2
#define DHTTYPE DHT22

// Initialize DHT22 sensor.
DHT dht(DHTPIN, DHTTYPE);
BME280 myBMP;

//The Nokia 5110 LCD
LCD5110 myGLCD(8,9,10,11,12);
    extern uint8_t SmallFont[];
    extern uint8_t MediumNumbers[];
    extern uint8_t BigNumbers[];
    const int bckLghtPin =  7;
    int bckLghtState = LOW;

// High - Low Temps
float maxTemp = 0;
float minTemp = 999;

void setup() {
  // Configure the BME
  myBMP.settings.runMode = 3; //  3, Normal mode
  myBMP.settings.tStandby = 5; //  0, 0.5ms
  myBMP.settings.filter = 0; //  0, filter off
  //tempOverSample:
  //  0, skipped
  //  1 through 5
  myBMP.settings.tempOverSample = 2;
  //pressOverSample:
  //  0, skipped
  //  1 through 5
  myBMP.settings.pressOverSample = 2;
  //humidOverSample:
  //  0, skipped
  //  1 through 5
  //mySensor.settings.humidOverSample = 1;
  myBMP.settings.I2CAddress = 0x76;
  
  Serial.begin(115200);
  delay(100); // allow sensor time to come up with new settings

  dht.begin();
  myBMP.begin();

   // set the digital pin as output:
   pinMode(bckLghtState, OUTPUT);
   pinMode(bckLghtPin, OUTPUT);
   
   // Setup the LCD
   myGLCD.InitLCD();
   myGLCD.setFont(SmallFont);
   myGLCD.setContrast(60);
}

void loop() {
  analogWrite(bckLghtPin, 255);
  
  // Wait a few seconds between measurements.
  delay(5000);
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float dhum = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float dtemp = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float dtfar = dht.readTemperature(true);
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(dtfar, dhum);
  // Compute heat index in Celsius (isFahreheit = false)
  // float hic = dht.computeHeatIndex(t, h, false);
  
  // Vars for BME280 Sensor readings
  int btempf = myBMP.readTempF();
  float bpres = myBMP.readFloatPressure();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(dhum) || isnan(dtfar)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print data reading to the LCD
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myGLCD.print("TFM Labs Weather", 0, 0);
  myGLCD.print("Temp= ", 0, 16);
  myGLCD.printNumI(dtfar, 37, 16);
  myGLCD.print("*F", 55, 16);
  myGLCD.print("Hum= ", 0, 26);
  myGLCD.printNumI(dhum, 37, 26);
  myGLCD.print("%", 55, 26);
  myGLCD.print("Pres= ", 0, 36);
  myGLCD.printNumI(bpres, 37, 36);
  myGLCD.print("atm", 54, 36);

  // Check High-Low
  if (dtfar > maxTemp) {
    maxTemp = dtfar;
  }
  if (dtfar < minTemp) {
    minTemp=dtfar;
  }
  // Project idea, add an LED to the board
  // turn it on if a max or min temp is reached

  // wait before changing screens
  delay(5000);
  myGLCD.clrScr();
  myGLCD.print("High Temp", CENTER, 0);
  myGLCD.printNumI(maxTemp, CENTER, 16);
  myGLCD.print("Low Temp", CENTER, 26);
  myGLCD.printNumI(minTemp, CENTER, 36);

  // Wait for it . . . 
  delay(5000);
  
  // Print data to serial using CVS format
  // for later analysis
  // save to SD card or ship over WiFI
  Serial.print(dtemp);
  Serial.print(",");
  Serial.print(dtfar);
  Serial.print(",");
  Serial.print(dhum);
  Serial.print(",");
  Serial.print(bpres);
  Serial.print(",");
  Serial.println(maxTemp);
  Serial.print(",");
}

