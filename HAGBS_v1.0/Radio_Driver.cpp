#include "Radio_Driver.h"

// Radio Ctor
Radio::Radio(){
  
}

void blink7(int time){
  pin_write(LED_PIN, HIGH);
  delay(time);
  pin_write(LED_PIN, LOW);
  delay(time);
}

//REQUIRES:  0 <= chan <= 15
//EFFECTS:   Changes radio's frequency channel to selected channel
void Radio::set_TX_chan(int chan){
  // Set channel
  Serial.write(String("CHAN 0" + String(getDigit(chan, 1)) + String(getDigit(chan, 0)) + '\r').c_str());
  //Serial.write(command);
  delay(55);
  currentChannel = chan;
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
void Radio::setChannelToFreq(int chan, float freq){
  // Convert channel to a two digit string
  String channel;
  if(chan < 10){
      channel = "0" + String(chan);
  }
  else{
      channel = String(chan);
  }
  
  int denominator = 10000000/R; // 10 MHz/R
  int N = (freq*1000000)/denominator; // N calculation, converting freq to Hz first

  // Set channel to frequency
  Serial.write(("LOAD " + channel + " " + String(N) + "\r").c_str());
  delay(55); // time for radio registers to set

}

//REQUIRES:  0 < R <= 16383, from Radio_driver.h
//EFFECTS:   Changes radio's R divider value, used for calculating the 
//           frequency the radio is set to
//MODIFIES:  Radio's R divider value register 
void Radio::set_R(){
  Serial.write(("RLOAD " + String(R) + "\r").c_str());
  //Serial.write(command);
  delay(55);
}

//EFFECTS: puts radio in serial listen mode to be able to be commanded
void Radio::serial_lock_radio(){
  Serial.write("LOCKSERIALMODE\r");
  delay(55);
}

void Radio::setup_radio(){ 
  Serial.begin(9600);
  delay(55);
  pinMode(AUDIO_OUT, OUTPUT);
  delay(55);
  pinMode(PTT, OUTPUT);
  delay(55);
  PTT_disable();
  serial_lock_radio();
  set_R();
  setChannelToFreq(0, 144.55); // Testing frequency
  setChannelToFreq(1, 144.39); // North America, Colombia, Chile, Indonesia
  setChannelToFreq(2, 144.575); // New Zealand
  setChannelToFreq(3, 144.62); // South Korea
  setChannelToFreq(4, 144.64); // China
  setChannelToFreq(5, 144.66); // Japan
  setChannelToFreq(6, 144.80); // Europe, Africa, Western and Northern Asia, Costa Rica
  setChannelToFreq(7, 144.93); // Argentina, Paraguay, Uruguay
  setChannelToFreq(8, 145.01); // Panama, Venezuela
  setChannelToFreq(9, 145.175); // Australia
  setChannelToFreq(10, 145.525); // Thailand
  setChannelToFreq(11, 145.575); // Brazil
  setChannelToFreq(12, 145.825); // ISS
  setChannelToFreq(13, 144.65); // Testing frequency 1
  setChannelToFreq(14, 144.75); // Testing frequency 2

  set_TX_chan(0);
}

//EFFECTS: Raises PTT pin low to enable the radio TXD pin
void Radio::PTT_enable(){
  pin_write(PTT, LOW);
  delay(100);
}

//EFFECTS: Raises PTT pin high to disable the radio TXD pin
void Radio::PTT_disable(){
  pin_write(PTT, HIGH);
  delay(100);
}

//EFFECTS: Returns the nth digit of num (starts at 0)
char Radio::getDigit(const int32_t num, const int32_t n){
  int32_t r = pow(10, n);
  int32_t x = (num / r) % 10;
  return x + '0';
}
