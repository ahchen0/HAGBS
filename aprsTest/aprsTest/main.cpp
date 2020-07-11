//
//  main.cpp
//  aprsTest
//
//  Created by Alex Chen on 11/20/19.
//  Copyright © 2019 Alex Chen. All rights reserved.
//

#include <iostream>
#include <math.h>
using namespace std;

char getDigit(int num, int n)
{
  int r, t1, t2;

  t1 = pow(10, n+1);
  r = num % t1;

  if (n > 0)
  {
    t2 = pow(10, n);
    r = r / t2;
  }

  return r + '0';
}

int main(int argc, const char * argv[]) {

    // Format UBlox GPS latitude
    char ublox_gps_lat[9];
    int32_t lat = 422933067;
    uint32_t ulat;
    char dir;
    if(lat >= 0){
      ulat = lat;
      dir = 'N';
    }
    else{
      ulat = -lat;
      dir = 'S';
    }
    int minutes = (ulat - (ulat/10000000)*10000000) * 60;
    ublox_gps_lat[0] = getDigit(ulat, 8);
    ublox_gps_lat[1] = getDigit(ulat, 7);
    ublox_gps_lat[2] = getDigit(minutes, 7);
    ublox_gps_lat[3] = getDigit(minutes, 6);
    ublox_gps_lat[4] = '.';
    ublox_gps_lat[5] = getDigit(minutes, 5);
    ublox_gps_lat[6] = getDigit(minutes, 4);
    ublox_gps_lat[7] = dir;
    ublox_gps_lat[8] = '\0';
    
    // Format UBlox GPS longitude
    char ublox_gps_lon[10];
    int32_t lon = -837119905;
    uint32_t ulon;
    if(lon >= 0){
      ulon = lon;
      dir = 'E';
    }
    else{
      ulon = -lon;
      dir = 'W';
    }
    minutes = (ulon - (ulon/10000000)*10000000) * 60;
    ublox_gps_lon[0] = getDigit(ulon, 9);
    ublox_gps_lon[1] = getDigit(ulon, 8);
    ublox_gps_lon[2] = getDigit(ulon, 7);
    ublox_gps_lon[3] = getDigit(minutes, 7);
    ublox_gps_lon[4] = getDigit(minutes, 6);
    ublox_gps_lon[5] = '.';
    ublox_gps_lon[6] = getDigit(minutes, 5);
    ublox_gps_lon[7] = getDigit(minutes, 4);
    ublox_gps_lon[8] = dir;
    ublox_gps_lon[9] = '\0';
    
    cout << ublox_gps_lat << endl;
    cout << ublox_gps_lon << endl;
}
