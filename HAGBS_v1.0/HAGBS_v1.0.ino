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

// Mpide 22 fails to compile Arduino code because it stupidly defines ARDUINO 
// as an empty macro (hence the +0 hack). UNO32 builds are fine. Just use the
// real Arduino IDE for Arduino builds. Optionally complain to the Mpide
// authors to fix the broken macro.
#if (ARDUINO + 0) == 0
#error "Oops! We need the real Arduino IDE (version 22 or 23) for Arduino builds."
#error "See trackuino.pde for details on this"

// Refuse to compile on arduino version 21 or lower. 22 includes an 
// optimization of the USART code that is critical for real-time operation
// of the AVR code.
#elif (ARDUINO + 0) < 22
#error "Oops! We need Arduino 22 or 23"
#error "See trackuino.pde for details on this"

#endif


// Trackuino custom libs
#include "config.h"
#include "afsk_avr.h"
#include "afsk_pic32.h"
#include "aprs.h"
#include "pin.h"
#include "power.h"

// HA-GBS libs
#include "Adafruit_BNO055.h"
#include "SparkFun_Ublox_Arduino_Library.h"
#include "Radio_Driver.h"
#include "rm3100.h"
#include "Geofence.h"
#include "ltc2309.h"

// Watchdog Timer lib
#include <avr/wdt.h>

#define HAGBS

// Arduino/AVR libs
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
// Module constants
static const uint32_t VALID_POS_TIMEOUT = 10000;  // ms

// Though not used here, we need to include Wire.h in this file for other code:
#include <Wire.h>

// Module variables
static int32_t next_aprs = 0;

// HA-GBS global variables
SFE_UBLOX_GPS* gps;
Adafruit_BNO055* bno;
//GeoFence* geofence;
LTC2309* adc;

// Time variables
int32_t lastTransmissionTime = 0;
#define transmissionPeriod 5000 // milliseconds

// TX Count
uint32_t TX_count;

void blink(int time){
  pin_write(LED_PIN, HIGH);
  delay(time);
  pin_write(LED_PIN, LOW);
  delay(time);
}

void watchdogSetup(void) {
  cli();
  wdt_reset();
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  WDTCSR = (1<<WDIE) | (1<<WDE) | (1<<WDP3) | (0<<WDP2) | (0<<WDP1) | (1<<WDP0);
  sei();
}

void setup()
{  
  // Enable watchdog
  watchdogSetup();
  
  // Configure LED PIN for debugging
  pinMode(LED_PIN, OUTPUT);
  pin_write(LED_PIN, LOW);
  blink(250);
  blink(250);
  blink(250);

  // Start i2c
  Wire.begin();

  // Initialze TX Count
  TX_count = 0;

  // deactivate internal pull-ups for twi
  // as per note from atmega8 manual pg167
  cbi(PORTC, 4);
  cbi(PORTC, 5);

  // Set up peripherals
  Serial.begin(GPS_BAUDRATE);
  gps_setup();
  afsk_setup();
  bno = new Adafruit_BNO055(55);
  bno->begin(Adafruit_BNO055::OPERATION_MODE_NDOF);
  RM3100_setup();
  adc = new LTC2309();

  // Set up geofence
  //GeoFence* geofence = new GeoFence();

  // Start transmission clock
  lastTransmissionTime = millis();
}

void loop()
{
  // Reset watchdog timer
  wdt_reset();
  
  // Time for another APRS frame
  if ((int32_t) (millis() - lastTransmissionTime) >= transmissionPeriod) {
    pin_write(LED_PIN, HIGH);
    aprs_send(); // All requests for data from sensors goes in here
    lastTransmissionTime = millis();
    while (afsk_flush()) {
      power_save();
    }
    pin_write(LED_PIN, LOW);
  }

  power_save(); // Incoming GPS data or interrupts will wake us up
}
