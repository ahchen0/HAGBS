//
//  geofence.hpp
//  
//
//  Created by Alex Chen on 10/15/19.
//

#ifndef geofence_hpp
#define geofence_hpp

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#endif /* geofence_hpp */

#define MAXREGIONS 22 // UPDATE THIS WHEN YOU DEFINE NEW REGIONS

using namespace std;

/*
 Intended Usage:
 1. Initialize with starting position
 2. Call getFreq with new position every time you need to transmit to get the right frequency
 */

class GeoFence{
private:
    
    struct Region{
        int channel; // Mhz
        float minLong; // Degrees
        float maxLong; // Degrees
        float minLat; // Degrees
        float maxLat; // Degrees
    };
    
    float currentLong;
    float currentLat;
    Region regions[MAXREGIONS];
    int numRegions;
    
    
    void addRegion(uint8_t channel, float minLong, float maxLong, float minLat, float maxLat);
    
    // Determine what region the vehicle is currently in
    void updateRegion();
    
    // Determines if vehicle has exited the last known region
    bool hasExitedRegion(float newLong, float newLat);
    
public:
    int currentRegion; // Index of regions vector
    
    // Initialize Geofence with the current position and list of regions
    GeoFence();
    
    // Get the channel you need to transmit on
    // Returns 0 if frequency is unknown
    uint8_t updatePositionAndGetChannel(float newLong, float newLat);
};
