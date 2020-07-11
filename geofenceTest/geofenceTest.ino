#include "geofence.h"
#include "Arduino.h"

GeoFence* geofence;

void setup() {
  Serial.begin(9600);
  geofence = new GeoFence();
  Serial.println(geofence->currentRegion);
}

void loop() {
  int channel = geofence->updatePositionAndGetChannel(-83, 42);
  Serial.println(channel);
  channel = geofence->updatePositionAndGetChannel(8, 50);
  Serial.println(channel);
  channel = geofence->updatePositionAndGetChannel(-60, -30);
  Serial.println(channel);
  channel = geofence->updatePositionAndGetChannel(0, -90);
  Serial.println(channel);
  delay(5000);
}


    
