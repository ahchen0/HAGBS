#include "Radio_Driver.h"
#include <string>

// Radio Ctor
Radio::Radio(): R(2000){
    setup_radio();
}

//REQUIRES:  0 <= chan <= 15
//EFFECTS:   Changes radio's frequency channel to selected channel
void Radio::set_TX_chan(int chan, char* command){
    /*// Check that channel is in range
    if(chan < 0 || chan > 15){
        return;
    }*/
    
    // Convert channel to a three digit string
    //char channel[4];
    command[0] = 'C';
    command[1] = 'H';
    command[2] = 'A';
    command[3] = 'N';
    command[4] = ' ';
	command[5] = '0';
    command[6] = getDigit(chan, 1);
    command[7] = getDigit(chan, 0);
	command[8] = '\r';
    command[9] = '\0';
    
    // Set channel
    //Serial.write(("CHAN " + channel).c_str());
	//Serial.write(command);
    //delay(55);
}


//REQUIRES:  0 <= chan <= 15; 
//           freq is a frequency integer in Megahertz in a range acceptable by radio
//           e.g. if setChannelToFreq(00,144.390) -> set channel 00 to 144.390 MHz
//                if setChannelToFreq(00,144.395) -> set channel 00 to 144.395 MHz
//           and the selected resolution determined by set the R divider value
//EFFECTS:   Sets radio's channel to a desired frequency by the following equation:
// --------------   RADIO FREQUENCY FORMULA ---------------------------------------
//           N = channel frequency / (10MHz / R)
//           N can be in range (0,65535]
//           See J.Schachter's script for calculating R to get a desired resolution
// --------------   COMMAND TO SEND  ----------------------------------------------
//           LOAD aa nnnnn (where aa is a channel # and nnnnn is N, from above)
void Radio::setChannelToFreq(int chan, float freq, char* command){
    // Convert channel to a two digit string
    //char command[15];
	command[0] = 'L';
	command[1] = 'O';
	command[2] = 'A';
	command[3] = 'D';
	command[4] = ' ';
    command[5] = getDigit(chan, 1);
    command[6] = getDigit(chan, 0);
    
    int denominator = 10000000/R; // 10 MHz/R
    int N = (freq*1000000)/denominator; // N calculation, converting freq to Hz first

	command[7] = ' ';
    command[8] = getDigit(N, 4);
    command[9] = getDigit(N, 3);
    command[10] = getDigit(N, 2);
    command[11] = getDigit(N, 1);
    command[12] = getDigit(N, 0);
    command[13] = '\r';
    command[14] = '\0';

    // Set channel to frequency
    // Serial.write(("LOAD " + channel + " " + String(N) + "\r").c_str());
    //Serial.write(command);
    //delay(55); // time for radio registers to set
}

//REQUIRES:  0 < R <= 16383, from Radio_driver.h
//EFFECTS:   Changes radio's R divider value, used for calculating the 
//           frequency the radio is set to
//MODIFIES:  Radio's R divider value register 
void Radio::set_R(char* command){
    command[0] = 'R';
    command[1] = 'L';
    command[2] = 'O';
    command[3] = 'A';
    command[4] = 'D';
    command[5] = ' ';
    command[6] = getDigit(R, 3);
    command[7] = getDigit(R, 2);
    command[8] = getDigit(R, 1);
    command[9] = getDigit(R, 0);
    command[10] = '\r';
    command[11] = '\0';
    //Serial.write(("RLOAD " + String(R) + "\r").c_str());
    //Serial.write(command);
    //delay(55);
}

// NOT NEEDED
/*void Radio::set_N(int N){
    Serial.write(("NLOAD " + String(N) + "\r").c_str()); // NLOAD???
    delay(55);
}*/

//EFFECTS: puts radio in serial listen mode to be able to be commanded
/*void Radio::serial_lock_radio(){
    Serial.write("LOCKSERIALMODE\r");
    delay(55);
}*/

void Radio::setup_radio(){
    /*Serial.begin(9600);
    pinMode(AUDIO_OUT, OUTPUT);
    pinMode(PTT, OUTPUT);
    serial_lock_radio();*/
    char* temp = new char[13];
    set_R(temp);
    delete[] temp;
}

//EFFECTS: Raises PTT pin low to enable the radio TXD pin
/*void Radio::PTT_enable(){
    digitalWrite(PTT, LOW);
}*/

//EFFECTS: Raises PTT pin high to disable the radio TXD pin
/*void Radio::PTT_disable(){
    digitalWrite(PTT, HIGH);
}*/

//EFFECTS: Returns the nth digit of num (starts at 0)
char Radio::getDigit(int num, int n)
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
