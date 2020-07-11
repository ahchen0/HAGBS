/* trackuino copyright (C) 2010  EA5HAV Javi
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "config.h"
#include "ax25.h"
//#include "gps.h"
#include "aprs.h"
#include "afsk_avr.h"
#include "afsk_pic32.h"
//#include "sensors_avr.h"   ---> commented out from heritage tracksoar code
//#include "sensors_pic32.h" ---> commented out from heritage tracksoar code
#include "Adafruit_BNO055.h"
#include "Adafruit_Sensor.h"
#include "rm3100.h"
#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif

#ifndef HAGBS
#include "SparkFun_Ublox_Arduino_Library.h"
#endif

#include "pin.h"
#include "GeoFence.h"
#include "Radio_Driver.h"
#include "ltc2309.h"

// Helper function stubs:
void gps_ax25_send_data();
void bno_ax25_send_data();
void rm3100_ax25_send_data();
void ltc2309_ax25_send_data();

// Blink function for debugging only:
void blink5(int time){
  pin_write(LED_PIN, HIGH);
  delay(time);
  pin_write(LED_PIN, LOW);
  delay(time);
}

// External pointers
extern SFE_UBLOX_GPS* gps;
extern Adafruit_BNO055* bno;
GeoFence* geofence;
extern Radio radio;
extern int channel;
extern LTC2309* adc;

// Module functions
float meters_to_feet(float m)
{
  // 10000 ft = 3048 m
  return m / 0.3048;
}

// TX count
extern uint32_t TX_count;

// NOTE: added in PBR params to save memory by making PBV copies
uint8_t getDigit(const int32_t &num, const int32_t &n){
  int32_t r = pow(10, n);
  int32_t x = (num / r) % 10;
  return x;
}


// Exported functions
void aprs_send()
{
  // NOTE: This is not an ideal way to initialize the geofence.
  // Please change this in future versions.
  if(TX_count == 0){
    geofence = new GeoFence();
  }
  
  //char temp[12];                   // Temperature (int/ext)
  const struct s_address addresses[] = { 
    {D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign
    {S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)
#ifdef DIGI_PATH1
    {DIGI_PATH1, DIGI_PATH1_TTL}, // Digi1 (first digi in the chain)
#endif
#ifdef DIGI_PATH2
    {DIGI_PATH2, DIGI_PATH2_TTL}, // Digi2 (second digi in the chain)
#endif
  };

  // Send AX.25 packet header:
  ax25_send_header(addresses, sizeof(addresses)/sizeof(s_address));

  // Call SOH_sensor/payload ax25 data formatting funcs:
  // These functions collect data from sensors and send to ax25 frame
  gps_ax25_send_data(); 
  ltc2309_ax25_send_data();           // ADC (chan 0-6)
  bno_ax25_send_data();               // IMU (accel, gyro, Mag1)
  rm3100_ax25_send_data();            // Magnetometer #2 (Mag2)

  // Send TX count
  TX_count++;
  ax25_send_string(String(TX_count).c_str());
  ax25_send_byte(',');
  
  ax25_send_string(APRS_COMMENT);     // Custom comment
  
  ax25_send_footer();
  ax25_flush_frame();                 // Tell the modem to go...YEET THAT DATA
}

void gps_ax25_send_data() {
  // Format UBlox GPS time
  gps->getPVT();
  uint8_t day = gps->getDay();
  uint8_t hour = gps->getHour();
  uint8_t minute = gps->getMinute();
  char ublox_gps_time[7]; // 'HHMMSS'
  snprintf(ublox_gps_time, 7,"%u%u%u%u%u%u", getDigit(day, 1), getDigit(day, 0), 
       getDigit(hour, 1), getDigit(hour, 0), getDigit(minute,1), getDigit(minute, 0));

  // Format UBlox GPS latitude
  char ublox_gps_lat[9];
  int32_t lat = gps->getLatitude();
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
  int32_t minutes = (ulat - int32_t(ulat/10000000)*10000000) * 60;
  snprintf(ublox_gps_lat, 9, "%d%d%d%d.%d%d%c", getDigit(ulat, 8), getDigit(ulat, 7),
           getDigit(minutes, 8), getDigit(minutes, 7), getDigit(minutes, 6), getDigit(minutes, 5), dir);

  // Format UBlox GPS longitude
  //char ublox_gps_lon[10] = {'0', '8', '3', '1', '2', '.', '3', '4', 'W'};
  char ublox_gps_lon[10];
  int32_t lon = gps->getLongitude();
  uint32_t ulon;
  if(lon >= 0){
    ulon = lon;
    dir = 'E';
  }
  else{
    ulon = -lon;
    dir = 'W';
  }
  minutes = (ulon - int32_t(ulon/10000000)*10000000) * 60;
  snprintf(ublox_gps_lon, 10, "%d%d%d%d%d.%d%d%c", getDigit(ulon, 9), getDigit(ulon, 8), getDigit(ulon, 7),
           getDigit(minutes, 8), getDigit(minutes, 7), getDigit(minutes, 6), getDigit(minutes, 5), dir);
 
  ax25_send_byte('/');                 // Report w/ timestamp, no APRS messaging. $ = NMEA raw data
  //ax25_send_string("021709z");       // 021709z = 2nd day of the month, 17:09 zulu (UTC/GMT)
  ax25_send_string(ublox_gps_time);    // 170915 = 17h:09m:15s zulu (not allowed in Status Reports)
  ax25_send_byte('z');
  ax25_send_string(ublox_gps_lat);     // Lat: 38deg and 22.20 min (.20 are NOT seconds, but 1/100th of minutes)
  ax25_send_byte('/');                 // Symbol table
  ax25_send_string(ublox_gps_lon);     // Lon: 000deg and 25.80 min  
  ax25_send_byte('O');                 // Symbol: O=balloon, -=QTH
  ax25_send_byte('/');

  // Send Altitude
  int32_t alt = gps->getAltitude();
  ax25_send_string(String(alt).c_str());
  ax25_send_byte(',');

  //information section:
  ax25_send_byte(gps->getFixType() + '0');
  int siv = gps->getSIV();
  ax25_send_byte(',');
  ax25_send_string(String(siv).c_str());
  //ax25_send_int(siv, 2);

  // If there are enough GPS sats in view, conduct geofencing:
  siv = 4;
  lon = -837137000;
  lat = 422935000;
  if(siv >= 4){
    //blink5(500);
    int targetChannel = geofence->updatePositionAndGetChannel(float(lon)/10000000, float(lat)/10000000);
    ax25_send_byte(',');
    ax25_send_string(String(targetChannel).c_str());
    //blink5(500);
    afsk_set_TX_chan(targetChannel);
    //blink5(500);
  }
}

void bno_ax25_send_data() {
  // Temperature (BNO)
  char temp[12];
  int8_t bno_temperature = bno->getTemp();
  ax25_send_string(String(bno->getTemp()).c_str());
  //ax25_send_int(bno->getTemp(), 2);
    
  // Accel data (BNO)
  //sensors_event_t*  bnoData = new sensors_event_t;
  sensors_event_t bnoData;
  bno->getEvent(&bnoData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  ax25_send_byte(',');
  ax25_send_string(String(bnoData.acceleration.x).c_str());
  //ax25_send_int(bnoData.acceleration.x * 1000, 5);
  ax25_send_byte(',');
  ax25_send_string(String(bnoData.acceleration.y).c_str());
  //ax25_send_int(bnoData.acceleration.y * 1000, 5);
  ax25_send_byte(',');
  ax25_send_string(String(bnoData.acceleration.z).c_str());
  //ax25_send_int(bnoData.acceleration.z * 1000, 5);
  
  // Gyro data (BNO)
  bno->getEvent(&bnoData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  ax25_send_byte(',');
  ax25_send_string(String(bnoData.gyro.x).c_str());
  //ax25_send_int(bnoData.gyro.x * 1000, 5);
  ax25_send_byte(',');
  ax25_send_string(String(bnoData.gyro.y).c_str());
  //ax25_send_int(bnoData.gyro.y * 1000, 5);
  ax25_send_byte(',');
  ax25_send_string(String(bnoData.gyro.z).c_str());
  //ax25_send_int(bnoData.gyro.z * 1000, 5);
  
  // Mag1 data (BNO)
  bno->getEvent(&bnoData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
  ax25_send_byte(',');
  ax25_send_string(String(bnoData.magnetic.x).c_str());
  //ax25_send_int(bnoData.magnetic.x * 1000, 5);
  ax25_send_byte(',');
  ax25_send_string(String(bnoData.magnetic.y).c_str());
  //ax25_send_int(bnoData.magnetic.y * 1000, 5);
  ax25_send_byte(',');
  ax25_send_string(String(bnoData.magnetic.z).c_str());
  //ax25_send_int(bnoData.magnetic.z * 1000, 5);
}

void rm3100_ax25_send_data() {
  // Mag2 data (RM3100)
  requestSingleMeasurement();
  sensor_xyz raw = ReadRM3100Raw();
  float gain = getRM3100Gain();
  float converted_x = (float) raw.x / gain;
  float converted_y = (float) raw.y / gain;
  float converted_z = (float) raw.z / gain;
  ax25_send_byte(',');
  ax25_send_string(String(converted_x).c_str());
  //ax25_send_int(converted_x * 1000, 5);
  ax25_send_byte(',');
  ax25_send_string(String(converted_y).c_str());
  //ax25_send_int(converted_y * 1000, 5);
  ax25_send_byte(',');
  ax25_send_string(String(converted_z).c_str());
  //ax25_send_int(converted_z * 1000, 5);
  ax25_send_byte(',');
}

void ltc2309_ax25_send_data() {
  // ADC data (8 channels)
  uint16_t data[NUM_CHANNELS];
  adc->readAllChannels(data);
  for(int i = 0; i < NUM_CHANNELS; i++){
    ax25_send_byte(',');
    ax25_send_string(String(data[i]).c_str());
    //ax25_send_int(data[i], 4);
  }
  ax25_send_byte(',');
}
