//
//  ltc2309.h
//  
//
//  Created by Alex Chen on 11/26/19.
//

#ifndef ltc2309_h
#define ltc2309_h

#include <stdio.h>

#endif /* ltc2309_h */

#define LTC2309_ADDRESS 0x08
#define NUM_CHANNELS 7

class LTC2309{
private:
  const uint8_t polarity = 1; // 1 for unipolar

  // CONFIGS (single-ended)
  uint8_t CONFIGS[NUM_CHANNELS] = 
  {
      0x08, /**< Ch0, referenced to COM */
      0x0C, /**< Ch1, referenced to COM */
      0x09, /**< Ch2, referenced to COM */
      0x0D, /**< Ch3, referenced to COM */
      0x0A, /**< Ch4, referenced to COM */
      0x0E, /**< Ch5, referenced to COM */
      0x0B, /**< Ch6, referenced to COM */
      //0x0F  /**< Ch7, referenced to COM */
  };

  int16_t readChannel(uint8_t channelCnfg, uint8_t p);

public:
  LTC2309();
  void readAllChannels(uint16_t* measurement);
  
};
