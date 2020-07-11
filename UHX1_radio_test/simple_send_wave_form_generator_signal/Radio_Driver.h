#include <Arduino.h>
#include <math.h>

// Pins
//#define AUDIO_OUT 3 // Selected heritage from TrackSoar
//#define PTT 4		// Selected heritage from TrackSoar

const int AUDIO_OUT = 3;
const int PTT = 4;

class Radio{

private:	
	char getDigit(int num, int digit);
  const int LED_PIN = 12;
    
public:
	Radio();

  void blink2(int time){
    digitalWrite(LED_PIN, HIGH);
    delay(time);
    digitalWrite(LED_PIN, LOW);
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
