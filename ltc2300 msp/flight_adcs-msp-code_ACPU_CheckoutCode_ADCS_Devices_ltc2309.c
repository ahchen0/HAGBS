/******************************************
 *  ____    ____   ____  ____   _____     *
 * |_   \  /   _| |_  _||_  _| |_   _|    *
 *   |   \/   |     \ \  / /     | |      *
 *   | |\  /| |      > `' <      | |   _  *
 *  _| |_\/_| |_   _/ /'`\ \_   _| |__/ | *
 * |_____||_____| |____||____| |________| *
 *                                        *
 ******************************************/

/** Driver for the LTC2309 ADC.
 * C implementation of code written by Alex Sloboda
 * Adapted by: Prince Kuevor
 *             kuevpr@umich.edu
 */

#include "ltc2309.h"

///////////////////////////////////////////////////////////////////////////////

/** Channel configurations for single-ended and differential modes. 
 * See Table 1 on page 11 of the datasheet. Index this array first using the
 * ChannelMode enumeration, then the channel number.
 */
const uint8_t ChannelConfigs[NUMBER_OF_MODES][NUMBER_OF_CHANNELS] = {

    // Differential configurations
    {
        0x00, /**< Ch0, referenced to Ch1 */
        0x01, /**< Ch2, referenced to Ch3 */
        0x02, /**< Ch4, referenced to Ch5 */
        0x03, /**< Ch6, referenced to Ch6 */
        0x04, /**< Ch1, referenced to Ch0 */
        0x05, /**< Ch3, referenced to Ch2 */
        0x06, /**< Ch5, referenced to Ch4 */
        0x07  /**< Ch7, referenced to Ch6 */
    },

    // Single-ended configurations
    {
        0x08, /**< Ch0, referenced to COM */
        0x0C, /**< Ch1, referenced to COM */
        0x09, /**< Ch2, referenced to COM */
        0x0D, /**< Ch3, referenced to COM */
        0x0A, /**< Ch4, referenced to COM */
        0x0E, /**< Ch5, referenced to COM */
        0x0B, /**< Ch6, referenced to COM */
        0x0F  /**< Ch7, referenced to COM */
    }
};

///////////////////////////////////////////////////////////////////////////////
/** measureAll_ltc2309
 * Iterates through the first 5 channels storing the output of each in 'measurement'
 * For CADRE, the only ADCS device that uses this driver only needs the first 5 channels
 *
 * Parameters:
 *    s_o          -> 0x01 for Single-ended, 0x00 for Differential
 *    pol          -> 0x01 for Unipolar, 0x00 for Bipolar 
 *    measurement  -> Pointer to memory with space for 8 shorts
 *                    Output data will be stored here.
*    For CADRE's TCB ADC: s_o = 1, pol = 0.
 * Return:
 *    A single byte whose bits correspond to which channel had the issue
 *    e.g. Channel 0's error bit is lsb, Channel 7's error bit is msb
 */
uint8_t measureAll_ltc2309_CADRE (uint8_t s_o, uint8_t pol, int16_t *measurement){
  uint8_t ret = 0x00;
  for(uint8_t i = 0;  i < 5; ++i, ++measurement){
    if( measureChannel_ltc2309(ChannelConfigs[s_o][i], pol, measurement))
      ret &= (0x01 << i);
  }
  
  return ret;
} // measureAll_ltc2309_CADRE

///////////////////////////////////////////////////////////////////////////////
/** measureAll_ltc2309
 * Iterates through all channels storing the output of each in 'measurement'
 *
 * Parameters:
 *    s_o          -> 0x01 for Single-ended, 0x00 for Differential
 *    pol          -> 0x01 for Unipolar, 0x00 for Bipolar 
 *    measurement  -> Pointer to memory with space for 8 shorts
 *                    Output data will be stored here.
*    For CADRE TCB ADC: s_o = 1, pol = 0.
 * Return:
 *    A single byte whose bits correspond to which channel had the issue
 *    e.g. Channel 0's error bit is lsb, Channel 7's error bit is msb
 */
uint8_t measureAll_ltc2309 (uint8_t s_o, uint8_t pol, int16_t *measurement){
  /*
  uint8_t channel   = 0x00,
          polarity  = 0x00,
          ret       = 0x00;
  
  channel &= 0x08;  // S/D bit (Single-Ended mode)
  polarity = 0x01;  // Unipolar mode
 */
  uint8_t ret = 0x00;
  for(uint8_t i = 0;  i < NUMBER_OF_CHANNELS; ++i, ++measurement){
    if( measureChannel_ltc2309(ChannelConfigs[s_o][i], pol, measurement))
      //prinf("values: %f", measurement[i]);
      ret &= (0x01 << i);
      
    /*
    channel &= (i % 2) << 2;  // O/S bit (odd or even);
    channel &= (i / 2);       // S1 and S0 bits (0 - 4)
    if( measureChannel_ltc2309(channel, polarity, measurement))
      ret &= (0x01 << i);
    */
  }
  
  return ret;
} // measureAll_ltc2309

///////////////////////////////////////////////////////////////////////////////
/** measureChannel_ltc2309
 * Constructs I2C command byte (page 10 of the datasheet)
 * and reads output response from the ADC. Prevents sleep mode.
 *
 * Parameters:
 *    channelCnfg  -> 4 most-significant bits.
 *                    Determins which channel to read from.
 *                    e.g. 0x08 is channel 0, 0x0D is channel 3
 *    p            -> Polarity bit. Succeeds channelCnfg bits.
 *                    Sets Uni/Bipolar. See pgs 10 and 15 for use.
 *                    1 for Unipolar, 0 for Bipolar
 *    measurement  -> Pointer to output data.
 * Return:
 *    TODO
 */
uint8_t measureChannel_ltc2309(uint8_t channelCnfg, uint8_t p, int16_t *measurement) {
  /* Measurement defaults to zero in case of error. */
  *measurement = 0;
  
  /* Build the I2C command byte.
   * - channelConfig is 4 bits, and occupies the most significant position.
   * - Uni/Bipolar bit is right after that
   * - For now, we'll leave the sleep bit unset (no sleeping).
   * - Last 2 bits are don't care
   * - see pages 10 and 16 of the datasheet for details.
   */
  const uint8_t sleep = 0; // don't sleep
  uint8_t cmdByte = (channelCnfg<<4) + (p<<3) + (sleep<<2);
  
  /* Write configuration as described on Page 16. A conversion is initiated
   * when the slave sees the STOP condition after the command byte
   * transmission.
   */
  if( !I2C_Write(&cmdByte, 1, 2) ) return 1;
  
  /* Read the conversion result as described on page 15. */
  uint8_t rxBytes[2];
  if( !(I2C_Read(rxBytes, 2, LTC2309_ADDRESS))) return 1;
  uint16_t rawResult = (rxBytes[0] << 8) + rxBytes[1];
  
  //TODO: Ensure this right shift is what we want
  /* Shift right to eliminate extra zeros at the end of the result. If
   * this LTC2309 driver is in bipolar mode, SampleType will be signed,
   * and this will be an arithmatic shift (preserves sign).
   */
  //Unipolar
  if( channelCnfg & 0x08){  
    *measurement = ((uint16_t) rawResult)  >>  4;
  }
  //Bipolar
  else{
    *measurement = ((int16_t) rawResult)  >>  4;
  }
  
  return 0;
} //measureChannel_ltc2309


        

/* Extra functions that aren't necesary, but might prove useful. 
 * They would require a global array to store the ADC data (ARRAY_NAME)
 * that would be accessed measurement_ltc2309 and written to by measure_ltc2309
///////////////////////////////////////////////////////////////////////////////
            
uint8_t measure_ltc2309 (uint32 channel, int16_t &measurement){
  int ret = measureChannel( ChannelConfigs[_channelMode][channel], _polarity, this->rawMeasurements[channel] );
  measurement = this->rawMeasurements[channel];
  return ret;    
} // measure_ltc2309
        
///////////////////////////////////////////////////////////////////////////////

int16_t measurement_ltc2309 (uint32_t channel){  
  return isValidChannel(channel) ? ARRAY_NAME[channel] : 0;
} // measurement_ltc2309

///////////////////////////////////////////////////////////////////////////////

double measurementVolts_ltc2309 (uint32_t channel){
  return measurement_ltc2309(channel) * VOLTS_PER_STEP;
} // measurementVolts_ltc2309
*/
///////////////////////////////////////////////////////////////////////////////
/** isValidChannel
 * Determines if input 'channel' is a valid channel number
 *
 * Parameters:
 *    channel  -> Channel number.
        
 * Return:
 *    True if input 'channel' is less than the numnber of channels on the ADC
 */
/*
bool isValidChannel (uint32_t channel){
  return channel < NUMBER_OF_CHANNELS;
}// isValidChannel
        */
///////////////////////////////////////////////////////////////////////////////
        
