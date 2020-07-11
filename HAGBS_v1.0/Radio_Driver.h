#include <Arduino.h>
#include <math.h>
#include "config.h"
#include "pin.h"

// Pins
//#define AUDIO_OUT 3 // Selected heritage from TrackSoar
//#define PTT 4		// Selected heritage from TrackSoar

const int AUDIO_OUT = 3;
const int PTT = 4;

class Radio{

private:	
	char getDigit(const int32_t num, const int32_t n);
  //const int LED_PIN = 7;
    
public:
  int currentChannel;

	Radio();

  void blink2(int time){
    pin_write(LED_PIN, HIGH);
    delay(time);
    pin_write(LED_PIN, LOW);
    delay(time);
  }
    
  void set_TX_chan(int chan);
  void setChannelToFreq(int chan, float freq);
  void set_R();
  void serial_lock_radio();
  void setup_radio();
  void PTT_enable();
  void PTT_disable();
  
  // -------------- Constant variables ---------------------------------------
  const int R = 2000; // 5kHz frequency resolution
  // Note: Setting R = 1000  -> 10kHz resolution
  //               R = 2000  ->  5kHz resolution (min. needed for global APRS)
  //               R = 10000 ->  1kHz resolution
  
  // const int AUDIO_OUT = 3; //Selected heritage from TrackSoar
  // const int PTT = 4;       //Selected heritage from TrackSoar
};
