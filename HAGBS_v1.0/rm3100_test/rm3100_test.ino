#include "rm3100.h"
#include "i2c.h"

sensor_xyz raw;
float gain;

void setup() {
  Serial.begin(9600);
  Serial.println("STARTING SETUP");
  i2c_init();
  RM3100_init_SM_Operation();
  re
  Serial.println("SETUP COMPLETE");
}

void loop() {
  requestSingleMeasurement();
  if(1){
    raw = ReadRM3100Raw();
    float converted_x = (float) raw.x / gain;
    float converted_y = (float) raw.y / gain;
    float converted_z = (float) raw.z / gain;
    Serial.print("X: ");
    Serial.println(converted_x);
    Serial.print("Y: ");
    Serial.println(converted_y);
    Serial.print("Z: ");
    Serial.println(converted_z);
    Serial.println();
  }
  else{
    Serial.println("DATA NOT READY\n");
  }
  delay(1000);
}
