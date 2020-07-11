//
//  ltc2309.cpp
//  
//
//  Created by Alex Chen on 11/26/19.
//

#include "ltc2309.h"
#include "Wire.h"
#include "pin.h"
#include "config.h"
#include "Arduino.h"

void blink11(int time){
  pin_write(LED_PIN, HIGH);
  delay(time);
  pin_write(LED_PIN, LOW);
  delay(time);
}

LTC2309::LTC2309(){
  //Wire.begin();
}

int16_t LTC2309::readChannel(uint8_t channelCnfg, uint8_t p){
  const uint8_t sleep = 0; // don't sleep
  uint8_t cmdByte = (channelCnfg<<4) + (p<<3) + (sleep<<2);

  // Request data from channel
  Wire.beginTransmission(LTC2309_ADDRESS);
  Wire.write(cmdByte);
  Wire.endTransmission();

  // Get data from channel
  int numBytes = Wire.requestFrom(LTC2309_ADDRESS, 2);
  uint8_t byte1 = Wire.read(); // Bit 1 of byte 1 is most significant bit
  uint8_t byte2 = Wire.read(); // Bit 4 of byte 2 is least significant bit
  uint16_t bothBytes = byte1 << 8 | byte2;
  uint16_t result = bothBytes >> 4;
  return result;
}

// Note: Only 7 channels used for HA-GBS, but I'm reading all 8 anyways
void LTC2309::readAllChannels(uint16_t* data){
  for(int i = 0; i < NUM_CHANNELS; i++){
    data[i] = readChannel(CONFIGS[i], polarity);
    delay(5);
  }
}
