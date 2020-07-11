#include <Arduino.h>
#include "SparkFun_Ublox_Arduino_Library.h"
#include <Wire.h>

SFE_UBLOX_GPS* gps;

void setup() {
  Serial.begin(9600);
  pinMode(12, OUTPUT);
  Serial.println("STARTING SETUP");
  gps_setup();
  Serial.println("SETUP COMPLETE");
}

void loop() {
  if(gps->getPVT()){
    int32_t latitude = gps->getLatitude();
    int32_t longitude = gps->getLongitude();
    Serial.print("Latitude: ");
    Serial.println(latitude);
    Serial.print("Longitude: ");
    Serial.println(longitude);
    Serial.println();
  }
  else{
    Serial.println("getPVT Failed");
    //Serial.print("SIV: ");
    //Serial.println(gps->getSIV());
    Serial.println();
  }
  delay(5000);
}
