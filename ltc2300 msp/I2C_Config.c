#include "I2C_Config.h"
#include "main.h"

const uint16_t TX_DEBUG_A = 0x0010;
const uint16_t TX_DEBUG_B = 0x0020;
const uint16_t TX_CONTROL = 0x0040;

unsigned char testByteRx = 0x00, testByteTx = 0x00;

uint32_t I2C_asMstrBase = USCI_B1_BASE;
uint32_t I2C_asSlvBase = USCI_B0_BASE; // New Probo
uint16_t crc_calc_i2c = INITIAL_CRC_VALUE;

//*****************************************************************************
//
//Specify number of bytes to be transmitted
//
//*****************************************************************************
uint8_t txLengthMstr;

uint8_t *RxBufferPointer;
uint8_t *TxBufferPointer;

uint8_t RxCounter;
uint8_t TxCounter;
bool init = false;  //True if currently communicating with FCPU over I2C. False Otherwise
bool crcChecked = false;
bool debugCmdChecked = false;
uint16_t prevState = 0x0000;
uint16_t fcpuRecvSize = 0;

unsigned long i2cTimer = 0;

//uint8_t debugBuff[MIN_DEBUG_MSG_SIZE];  //Buffer to send Debug Mode data of type A
FCPU_TxA_Debug firstMessage;

//In slave mode: for comm with FCPU (STAMP)
uint16_t txLengthSlv;           //TODO: make a uint8_t   
uint16_t slvIndx = 0;  // Current index of message being sent to or received from adcs_app
uint8_t transmitCounter;
uint8_t *transmitDataPointer;
uint8_t *receivedDataPointer;

//------------------------------------------------------------------------------
// void USCI_I2C_transmitinit(unsigned char slave_address)
//
// This function initializes the USCI module for master-transmit operation. 
//
// IN:   unsigned char slave_address   =>  Slave Address
//------------------------------------------------------------------------------
void USCI_I2C_init(unsigned char slave_address)
{
    
  //Set USCI_B1 pins for I2C (3.7 (SDA), 5.4 (SCL))
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN7);
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN4);

  //Initialize Master
  USCI_B_I2C_masterInit(I2C_asMstrBase,
                        USCI_B_I2C_CLOCKSOURCE_SMCLK,
                        UCS_getSMCLK(),
                        USCI_B_I2C_SET_DATA_RATE_100KBPS
                        );

  //Specify slave address
  USCI_B_I2C_setSlaveAddress(I2C_asMstrBase, slave_address);

  //Enable I2C Module to start operations
  USCI_B_I2C_enable(I2C_asMstrBase);
         
  //Enable transmit Interrupt
  USCI_B_I2C_clearInterruptFlag(I2C_asMstrBase,
                                USCI_B_I2C_TRANSMIT_INTERRUPT +
                                USCI_B_I2C_RECEIVE_INTERRUPT  
                                );
    
  USCI_B_I2C_enableInterrupt(I2C_asMstrBase,
                             USCI_B_I2C_NAK_INTERRUPT +
                             USCI_B_I2C_ARBITRATIONLOST_INTERRUPT +
                             USCI_B_I2C_STOP_INTERRUPT +
                             USCI_B_I2C_START_INTERRUPT +
                             USCI_B_I2C_TRANSMIT_INTERRUPT + 
                             USCI_B_I2C_RECEIVE_INTERRUPT
                             );
 
}

//------------------------------------------------------------------------------
// void USCI_I2C_transmitinit(unsigned char slave_address)
//
// This function initializes the USCI module for master-transmit operation. 
//
// IN:   unsigned char slave_address   =>  Slave Address
//------------------------------------------------------------------------------
void USCI_I2C_slvInit(void){
  
  //Set USCI_B0 pins for I2C (3.1 (SDA), 3.2 (SCL))
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN1 + GPIO_PIN2);
  
  //Initialize I2C as a slave device
  USCI_B_I2C_slaveInit(I2C_asSlvBase, SLAVE_ADDRESS_SELF);

  //Enable I2C Module to start operations
  USCI_B_I2C_enable(I2C_asSlvBase);
  
  //Enable interrupts
  USCI_B_I2C_clearInterruptFlag(I2C_asSlvBase,
                                USCI_B_I2C_TRANSMIT_INTERRUPT +
                                USCI_B_I2C_RECEIVE_INTERRUPT  
                                );
  USCI_B_I2C_enableInterrupt(I2C_asSlvBase,
                             USCI_B_I2C_TRANSMIT_INTERRUPT + 
                             USCI_B_I2C_RECEIVE_INTERRUPT
                             );
  
  
    USCI_B_I2C_setMode(I2C_asSlvBase, USCI_B_I2C_TRANSMIT_MODE);
    USCI_B_I2C_setMode(I2C_asSlvBase, USCI_B_I2C_RECEIVE_MODE);
    
  
    
    //Enable interrupts
    USCI_B_I2C_clearInterruptFlag(I2C_asSlvBase,
                                  USCI_B_I2C_TRANSMIT_INTERRUPT +
                                  USCI_B_I2C_RECEIVE_INTERRUPT  +
                                  USCI_B_I2C_STOP_INTERRUPT
                                  );
  
    USCI_B_I2C_enableInterrupt(I2C_asSlvBase,
                               USCI_B_I2C_TRANSMIT_INTERRUPT + 
                               USCI_B_I2C_RECEIVE_INTERRUPT +
                               USCI_B_I2C_STOP_INTERRUPT
                               );
    
   __enable_interrupt(); 
  
  init = false;
  crcChecked = false;
}// USCI_I2C_slvInit

//------------------------------------------------------------------------------
// void USCI_I2C_receive(unsigned char byteCount, unsigned char *field)
//
// This function is used to start an I2C commuincation in master-receiver mode. 
//
// IN:   unsigned char byteCount  =>  number of bytes that should be read
//       unsigned char *field     =>  array variable used to store received data
//------------------------------------------------------------------------------


bool I2C_Read(uint8_t * receiveBuffer, uint16_t byteCount, uint8_t slave_address){
        
        //__disable_interrupt();
        i2cTimer = 0;
        //Specify slave address
        USCI_B_I2C_setSlaveAddress(I2C_asMstrBase, slave_address);
        
        //wait for bus to be free
        // I DONT THINK THIS IS WORKING - Nate E
        while (USCI_B_I2C_isBusBusy(I2C_asMstrBase ) && i2cTimer < I2C_TIMEOUT)
          ++i2cTimer;
        
        if( i2cTimer >= I2C_TIMEOUT)
          return false;
        
        i2cTimer = 0;

        RxBufferPointer = receiveBuffer;
        RxCounter = byteCount;

        //Initialize multi reception
       USCI_B_I2C_masterMultiByteReceiveStart(I2C_asMstrBase);
       __enable_interrupt();
        
        //Delay until transmission completes
        while (USCI_B_I2C_isBusBusy(I2C_asMstrBase) && i2cTimer < I2C_TIMEOUT)
          ++i2cTimer;
        
        if( i2cTimer >= I2C_TIMEOUT)
          return false;

        return true;
}// I2C_Read

//------------------------------------------------------------------------------
// void USCI_I2C_transmit(unsigned char byteCount, unsigned char *field)
//
// This function is used to start an I2C commuincation in master-transmit mode. 
//
// RETURN:  TRUE if message is transmitted sucessfully
//          FALSE if function times out
//
// IN:   unsigned char byteCount  =>  number of bytes that should be transmitted
//       unsigned char *field     =>  array variable. Its content will be sent.
//------------------------------------------------------------------------------


bool I2C_Write(uint8_t* field, uint8_t byteCount, uint8_t slave_address){
        //Specify slave address
        USCI_B_I2C_setSlaveAddress(I2C_asMstrBase,slave_address);
        
        i2cTimer = 0;
        
        TxBufferPointer = field;
        txLengthMstr = byteCount;
  
        //Load TX byte counter
        TxCounter = 1;

        //Initiate start and send first character
        USCI_B_I2C_masterMultiByteSendStartWithTimeout(I2C_asMstrBase,
                                            *TxBufferPointer,
                                            1
                                            );
        __enable_interrupt();
        
        //Delay until transmission completes
        while (USCI_B_I2C_isBusBusy(I2C_asMstrBase) && i2cTimer < I2C_TIMEOUT)
          ++i2cTimer;
        
        if( i2cTimer >= I2C_TIMEOUT)
          return false;
        
        return true;
}// I2C_Write

//******************************************************************************
//
//This is the USCI_B1 interrupt vector service routine.
//
//******************************************************************************

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B1_VECTOR)))
#endif
void USCI_B1_ISR(void)
{
      switch (__even_in_range(UCB1IV, 12)) {
        case USCI_I2C_UCTXIFG:
        {
          //Check TX byte counter
          if (TxCounter < txLengthMstr) {
            USCI_B_I2C_masterMultiByteSendNextWithTimeout(I2C_asMstrBase,
                                                     *(TxBufferPointer+TxCounter),
                                                     1);
            //Increment TX byte counter
            TxCounter++;
          } 
          else {
            //Initiate stop only
            USCI_B_I2C_masterMultiByteSendStop(I2C_asMstrBase);
          }
          break;
        }
        
        case USCI_I2C_UCRXIFG:
        {
          //Decrement RX byte counter
          RxCounter--;

          if(RxCounter){
            if(RxCounter == 1){
                    //Initiate end of reception -> Receive byte with NAK
                    *RxBufferPointer++ =
                            USCI_B_I2C_masterMultiByteReceiveFinish(
                                    I2C_asMstrBase
                                    );
            }
            else{
              //Keep receiving one byte at a time
              *RxBufferPointer++ = USCI_B_I2C_masterMultiByteReceiveNext(
                                    I2C_asMstrBase
                                    );
            }
          }
          else{
            //Receive last byte
            *RxBufferPointer = USCI_B_I2C_masterMultiByteReceiveNext(
                                I2C_asMstrBase
                                );
          }
          break;    
        }
      }
}

//SLAVE MODE
//******************************************************************************
//This is the USCI_B0 interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCI_B0_ISR(void){
  switch (__even_in_range(UCB0IV, 12)) { 
    //Stop Condition (end of I2C message)
    case USCI_I2C_UCSTPIFG:  
      
      if(prevState == USCI_I2C_UCRXIFG){
        ACPU_Data_Store.recvFromFcpu = true;
        ACPU_Data_Store.all_comm_rx++;
        
        imuCount = 0;
        ACPU_Data_Store.status = 0;
        
        debugCmdChecked = false;
      }
      else
        ACPU_Data_Store.all_comm_tx++;
      
      LED2();
      init = false;
      crcChecked = false;
      
      break;
      
    //Transmit data
    case USCI_I2C_UCTXIFG:
      if(!init){ 
        slvIndx = 0;
        //Control Mode
        if((fcpu_rx.control.command & COMMAND_MODE) == MODE_CONTROL){
          transmitDataPointer = (uint8_t *)(&fcpu_tx.control);
          txLengthSlv = sizeof(fcpu_tx.control);
          prevState = TX_CONTROL;
        }
        //Debug Mode
        else if((fcpu_rx.control.command & COMMAND_MODE) == MODE_DEBUG){
          //Type A
          
          if(prevState != TX_DEBUG_A){

            memcpy(&firstMessage, 
                   (uint8_t *)(&fcpu_tx.debug.command), 
                   MIN_DEBUG_MSG_SIZE - sizeof(firstMessage.crc_send));
            
            memcpy(&(firstMessage.crc_send),
                   &fcpu_tx.debug.crc_send, 
                   sizeof(firstMessage.crc_send));
            
            transmitDataPointer = (uint8_t *) &(firstMessage);
            txLengthSlv = MIN_DEBUG_MSG_SIZE;
            prevState = TX_DEBUG_A;
          }
         
          //Type B
          else{
            transmitDataPointer = (uint8_t *)(&fcpu_tx.debug);
            txLengthSlv  = MIN_DEBUG_MSG_SIZE + fcpu_tx.debug.size;
            prevState = TX_DEBUG_B;
          }
        }
        init = true;
      }//if(!init)
      
      //If in control mode, and initialization has completed
      if (slvIndx < txLengthSlv){
        USCI_B_I2C_slaveDataPut(I2C_asSlvBase, transmitDataPointer[slvIndx]);
        ++slvIndx;
      } 
      /** This is just to do something so we don't go off the end of the array
       * The sent message will be incorrect, but this will be caught by the CRC.
       */
      else{
        slvIndx = 0;
        USCI_B_I2C_slaveDataPut(I2C_asSlvBase, transmitDataPointer[slvIndx]);
      }
      
      break;
      
    //Receive Data          
    case USCI_I2C_UCRXIFG:
      
      if(!init){
        fcpuRecvSize = sizeof(fcpu_rx.control);
        slvIndx = 0;
        
        receivedDataPointer = (uint8_t*)(&fcpu_rx.control);
        
        prevState = USCI_I2C_UCRXIFG;
        init = true;
      }
      
      if(slvIndx < fcpuRecvSize){
        receivedDataPointer[slvIndx] = USCI_B_I2C_slaveDataGet(I2C_asSlvBase);
        ++slvIndx;
      }
      /** This is just to do something so we don't go off the end of the array
       * The received message will be incorrect, but this will be caught by the CRC.
       */
      else{
        slvIndx = 0;
        receivedDataPointer[slvIndx] = USCI_B_I2C_slaveDataGet(I2C_asSlvBase);
      }
      
      break;  
      
    default:
      break;
      
  }//switch (__even_in_range)
}//USCI_B0_ISR()

