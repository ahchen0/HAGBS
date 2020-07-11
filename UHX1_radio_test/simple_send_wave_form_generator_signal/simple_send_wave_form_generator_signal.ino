 /*int AUDIO_OUT = 3;
 int PTT = 4;
 int FREQ_DESIRED = 144390000; //Hz
 int R = 2000;
 int N = FREQ_DESIRED/(10000000/R);
 String CHAN_DEFAULT = "00";
 String channel_load = "LOAD " + CHAN_DEFAULT + " " +  String(N) + "\r";*/

#include "Radio_Driver.h"
#include "Arduino.h"
//const int LED_PIN = 12;
const int PTT_PIN = 2;

void blink(int time){
  digitalWrite(PTT_PIN, HIGH);
  delay(time);
  digitalWrite(PTT_PIN, LOW);
  delay(time);
}
Radio radio;

void setup() {
  pinMode(PTT_PIN, OUTPUT);
  digitalWrite(PTT_PIN, HIGH);
  //blink(250);
  //blink(250);
  //blink(250);
  //blink(250);
  //radio.setup_radio();
  //radio.set_TX_chan(0);
  //radio.setChannelToFreq(0, 143.990);
  //Serial.write("LOAD 00 28798\r");
  //blink(1000);
  //radio.set_TX_chan(0);
  //blink(1000);
  //blink(2000);
}



void loop() {}
/*
void loop() {
  //radio->PTT_enable();
  //digitalWrite(LED_BUILTIN, HIGH);
  //delay(2000);
  //radio->PTT_disable();
  //digitalWrite(LED_BUILTIN, LOW);
  //delay(10000);
  //analogWrite(AUDIO_OUT, 100); // sends a   pulse
  //delay(2000);
  //radio->PTT_disable();
  blink(500);
  radio.PTT_enable();
  blink(500);
  analogWrite(AUDIO_OUT, 200); // sends a   pulse
  blink(500);
  radio.PTT_disable();
  delay(1000);
  
}*/

//#include <Arduino.h>


//int AUDIO_OUT = 3;
//int PTT = 4;
//int FREQ_DESIRED = 144390000; //Hz
//int R = 2000;
//int N = FREQ_DESIRED/(10000000/R);
//String CHAN_DEFAULT = "00";
//String channel_load = "LOAD " + CHAN_DEFAULT + " " +  String(N) + "\r";
/*
void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  blink(3000);
  Serial.begin(9600); //initialize serial port on CPU
  blink(1000);
  pinMode(AUDIO_OUT,OUTPUT);
  blink(1000);
  pinMode(PTT,OUTPUT);
  blink(1000);
  digitalWrite(PTT, HIGH);
  blink(1000);
  
  Serial.write("LOCKSERIALMODE\r"); //send serial mode cmd to radio over UART
  delay(55);
  Serial.write("RLOAD 2000\r"); //change default R value (see datasheet)
  delay(55);
  Serial.write("LOAD 00 28798\r"); //set default channel (00 to 144.380MHz)
  //delay(55);
  //Serial.write("LOAD 01 14439\r"); //set channel 01 to 144.39 MHz
  delay(55);
  
  //digitalWrite(PTT,LOW); // enable PTT (active LOW)

}
*/
//void loop() {
 //radio.PTT_enable();
 //digitalWrite(PTT_PIN, HIGH);
 //analogWrite(AUDIO_OUT, 250); // sends a pulse 
 //delay(200);
 //radio.PTT_disable();
 //digitalWrite(PTT_PIN, LOW);
 //delay(1000);
//}
