#include <math.h>
#include <iostream>

// Pins
#define AUDIO_OUT 3 // Selected heritage from TrackSoar
#define PTT 4		// Selected heritage from TrackSoar

class Radio{

// Commented out bc I (jhs) do not thing anything really needs hiding
// as we are never referencing any Radio parameters outside of making 
// serial command calls to change interal radio registers
private:
    //int N;
    //double power_level;
	
	char getDigit(int num, int digit);
    
public:
	Radio();
    
    void set_TX_chan(int chan, char* channel);
    void setChannelToFreq(int chan, float freq, char* command);
    void set_R(char* command);
    void serial_lock_radio();
    void setup_radio();
    void PTT_enable();
    void PTT_disable();
    
    // -------------- Constant variables ---------------------------------------
    int R; // 5kHz frequency resolution
    // Note: Setting R = 1000  -> 10kHz resolution
    //               R = 2000  ->  5kHz resolution (min. needed for global APRS)
    //               R = 10000 ->  1kHz resolution
    
    // const int AUDIO_OUT = 3; //Selected heritage from TrackSoar
    // const int PTT = 4;       //Selected heritage from TrackSoar
};
