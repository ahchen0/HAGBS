#ifndef __I2C_CONFIG_H__
#define __I2C_CONFIG_H__

#include "Task_FCPU_Update.h"
#include "CRC16.h"
#include "UCS_Config.h"

//Slave address of the ACPU
#define SLAVE_ADDRESS_SELF   0x3A

//Timeouts used for I2C Communication when ACPU is a master to the ADCS Devices
//#define I2C_TIMEOUT          1000  //About 7.5ms
//#define I2C_TIMEOUT          190   //About 1.4ms
//#define I2C_TIMEOUT           5000   // ~12ms with 18MHz
#define I2C_TIMEOUT           650   // ~12ms with 18MHz
#define I2C_SLV_TX_TO         300000
#define I2C_SLV_RX_TO         300000

extern uint16_t fcpuRecvSize;
extern bool crcChecked;
extern bool init;
extern bool debugCmdChecked;
/************************** Function Prototypes *******************************/
extern void USCI_I2C_init(uint8_t slave_address);
extern void USCI_I2C_slvInit(void);
extern bool I2C_slaveWrite(unsigned char* field, unsigned char byteCount);
extern bool I2C_slaveRead(unsigned char rxBuffer[], uint8_t byteCount);

void USCI_I2C_receive(uint8_t byteCount, uint8_t *field);
void USCI_I2C_transmit(uint8_t byteCount, uint8_t *field);

extern bool I2C_Read(uint8_t* receiveBuffer, uint16_t byteCount, uint8_t slave_address);
extern bool I2C_Write(uint8_t* field, uint8_t byteCount, uint8_t slave_address);   
extern uint8_t I2C_Single_Read(uint8_t slave_address);

extern void I2C_WriteByte(uint8_t slave_address, uint8_t sub_address, uint8_t Write_Value);

#endif