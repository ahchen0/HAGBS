//
//  geofence.cpp
//  
//
//  Created by Alex Chen on 10/15/19.
//

#include "geofence.h"
//#include "pin.h"
//#include "config.h"
#include "Arduino.h"

using namespace std;

GeoFence::GeoFence(){
    // Initializes current coordinates to South Pole so radio is set to default channel
    currentLong = 0;
    currentLat = -90;
    numRegions = 0;

    // For testing
    //addRegion(0, 0, 0, 0, 0); // Unknown
    //addRegion(1, -83.7158827, -83.7119475, 42.2921426, 42.2942333); // Orange parking lot (south of Hayward)
    //addRegion(1, -83.718, -83.7108, 42.29167, 42.2964); // GG Brown
    //addRegion(0, -83.7108, -83.71307, 42.2964, 42.3036); // NCRC
    //addRegion(1, -83.73, -83.7108, 42.28333, 42.29167); //Southern North Campus
    // Go to music school parking lot to test default

    // For Flight
    addRegion(0, 0, 0, 0, 0); // Unkown
    addRegion(1, -170, -60, 13, 90); // North America
    addRegion(6, -25, 75, -60, 90); // Europe, North Africa, Western Asia
    addRegion(6, 75, 180, 45, 90); // Russian Far East
    addRegion(6, -86, -83, 8, 12); // Costa Rica
    addRegion(8, -83, -77.5, 7, 10); // Panama
    addRegion(1, -79, -73, -4, 13); // Western Colombia
    addRegion(1, -73, -68, -4, 7); // Eastern Colombia
    addRegion(8, -73, -60, 6, 13); // Northern Venezuela
    addRegion(8, -68, -61, 1, 6); // Southern Venezuela
    addRegion(11, -70, -35, -20, 0); // Northern Brazil
    addRegion(11, -54, -40, -31, -20); // Southern Brazil
    addRegion(1, -75, -67, -60, -18); // Chile
    addRegion(9, -67, -53, -60, -19); // Argentina/Paraguay/Uruguay
    addRegion(4, 75, 125, 20, 45); // Most of China
    addRegion(4, 113, 130, 40, 53); // Northeast China
    addRegion(10, 97, 106, 5, 21); // Thailand
    addRegion(1, -10, 5, 95, 141); // Indonesia
    addRegion(9, 110, 160, -45, -10); // Australia
    addRegion(2, 165, 180, -50, -34); // New Zealand
    addRegion(3, 124, 130, 34, 39); // South Korea
    addRegion(5, 130, 147, 30, 45); // Japan
    
    updateRegion();
}

void GeoFence::addRegion(uint8_t channel, float minLong, float maxLong, float minLat, float maxLat){
    if(numRegions >= MAXREGIONS){
        return;
    }
    Region newRegion;
    newRegion.channel = channel;
    newRegion.minLong = minLong;
    newRegion.maxLong = maxLong;
    newRegion.minLat = minLat;
    newRegion.maxLat = maxLat;
    regions[numRegions] = newRegion;
    numRegions++;
}

void GeoFence::updateRegion(){
   for(unsigned short i = 1; i < numRegions; i++){
        if(currentLong > regions[i].minLong &&
           currentLong < regions[i].maxLong &&
           currentLat  > regions[i].minLat  &&
           currentLat  < regions[i].maxLat){
            currentRegion = i;
            return;
        }
    }
    currentRegion = 0;
}

bool GeoFence::hasExitedRegion(float newLong, float newLat){
    if(newLong < regions[currentRegion].minLong ||
       newLong > regions[currentRegion].maxLong ||
       newLat  < regions[currentRegion].minLat  ||
       newLat  > regions[currentRegion].maxLat){
        return true;
    }
    return false;
}

uint8_t GeoFence::updatePositionAndGetChannel(float newLong, float newLat){
    currentLong = newLong;
    currentLat = newLat;
    if(hasExitedRegion(newLong, newLat)){
        updateRegion();
    }
    return regions[currentRegion].channel;
}
