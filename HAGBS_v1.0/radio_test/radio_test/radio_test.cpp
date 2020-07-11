#include <iostream>
#include <math.h>
#include "Radio_Driver.h"
using namespace std;

int main(){
    Radio radio;
    
    // set_TX_chan test
    char* command = new char[10];
    radio.set_TX_chan(3, command);
    cout << command << endl;
    radio.set_TX_chan(13, command);
    cout << command << endl;
    delete[] command;
    
    // setChannelToFreq test
    command = new char[15];
    radio.setChannelToFreq(0, 143.99, command);
    cout << command << endl;
    radio.setChannelToFreq(14, 144.80, command);
    cout << command << endl;
    delete[] command;
    
    // set_R test
    command = new char[13];
    radio.set_R(command);
    cout << command << endl;
    delete[] command;
}
