/******************************************
 *  ____    ____   ____  ____   _____     *
 * |_   \  /   _| |_  _||_  _| |_   _|    *
 *   |   \/   |     \ \  / /     | |      *
 *   | |\  /| |      > `' <      | |   _  *
 *  _| |_\/_| |_   _/ /'`\ \_   _| |__/ | *
 * |_____||_____| |____||____| |________| *
 *                                        *
 ******************************************/

#ifndef __LTC2309_H__
#define __LTC2309_H__

#include "I2C_Config.h"

//Slave addresses are on Table 2 (pg 16) of the datasheet
#define LTC2309_ADDRESS     0x1A //0x18  //float, float
#define NUMBER_OF_CHANNELS  8
#define NUMBER_OF_MODES     2

//static const double VOLTS_PER_STEP = 1;

extern uint8_t measureAll_ltc2309 (uint8_t s_o, uint8_t pol, int16_t *measurement);
extern uint8_t measureChannel_ltc2309(uint8_t channelCnfg, uint8_t p, int16_t *measurement);
extern uint8_t measureAll_ltc2309_CADRE (uint8_t s_o, uint8_t pol, int16_t *measurement);

//static inline bool isValidChannel (uint32_t channel);

#endif