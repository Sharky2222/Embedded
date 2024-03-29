//this is from texas instruments

#include <msp430.h>


#include "TI_USCI_I2C_master.h"

signed char byteCtr;
unsigned char *TI_receive_field;
unsigned char *TI_transmit_field;


unsigned char I2C_NO_STOP_FLAG = 0x00;

void TI_USCI_I2C_receiveinit(unsigned char slave_address,
                             unsigned char prescale){

    P1SEL |= BIT6 + BIT7;                                                 // Assign I2C pins to USCI_B0
    P1SEL2|= BIT6 + BIT7;                                                // Assign I2C pins to USCI_B0


  //P3SEL |= SDA_PIN + SCL_PIN;                 // Assign I2C pins to USCI_B0
  UCB0CTL1 = UCSWRST;                        // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;       // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;              // Use SMCLK, keep SW reset
  UCB0BR0 = prescale;                         // set prescaler
  UCB0BR1 = 0;
  UCB0I2CSA = slave_address;                  // set slave address
  UCB0CTL1 &= ~UCSWRST;                       // Clear SW reset, resume operation
  UCB0I2CIE = UCNACKIE;
  IE2 = UCB0RXIE;                            // Enable RX interrupt

}

void TI_USCI_I2C_transmitinit(unsigned char slave_address,
                          unsigned char prescale){
  //P3SEL |= SDA_PIN + SCL_PIN;                 // Assign I2C pins to USCI_B0

    P1SEL |= BIT6 + BIT7;                                                 // Assign I2C pins to USCI_B0
    P1SEL2|= BIT6 + BIT7;                                                // Assign I2C pins to USCI_B0


  UCB0CTL1 = UCSWRST;                        // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;       // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;              // Use SMCLK, keep SW reset
  UCB0BR0 = prescale;                         // set prescaler
  UCB0BR1 = 0;
  UCB0I2CSA = slave_address;                  // Set slave address
  UCB0CTL1 &= ~UCSWRST;                       // Clear SW reset, resume operation
  UCB0I2CIE = UCNACKIE;
  IE2 = UCB0TXIE;                            // Enable TX ready interrupt
}
void TI_USCI_I2C_receive(unsigned char byteCount, unsigned char *field){
  TI_receive_field = field;
  if ( byteCount == 1 ){
    byteCtr = 0 ;
    __disable_interrupt();
    UCB0CTL1 |= UCTXSTT;                      // I2C start condition
    while (UCB0CTL1 & UCTXSTT);               // Start condition sent?
    UCB0CTL1 |= UCTXSTP;                      // I2C stop condition
    __enable_interrupt();
  } else if ( byteCount > 1 ) {
    byteCtr = byteCount - 2 ;
    UCB0CTL1 |= UCTXSTT;                      // I2C start condition
  } else
    while (1);                                // illegal parameter
}
void TI_USCI_I2C_transmit(unsigned char byteCount, unsigned char *field)
{
    //no stop flag used for disablel stop condition
    //after it's complete
    I2C_NO_STOP_FLAG = 0x00;    //clear the no stop always
    TI_transmit_field = field;
    byteCtr = byteCount;
    UCB0CTL1 |= UCTR + UCTXSTT;                 // I2C TX, start condition
}

void TI_USCI_I2C_transmit_nostop(unsigned char byteCount, unsigned char *field)
{
    //cleared in the isr in the last byte
    I2C_NO_STOP_FLAG = 1;

    TI_transmit_field = field;
    byteCtr = byteCount;
    UCB0CTL1 |= UCTR + UCTXSTT;                 // I2C TX, start condition
}
unsigned char TI_USCI_I2C_slave_present(unsigned char slave_address){
  unsigned char ie2_bak, slaveadr_bak, ucb0i2cie, returnValue;
  ucb0i2cie = UCB0I2CIE;                      // restore old UCB0I2CIE
  ie2_bak = IE2;                              // store IE2 register
  slaveadr_bak = UCB0I2CSA;                   // store old slave address
  UCB0I2CIE &= ~ UCNACKIE;                    // no NACK interrupt
  UCB0I2CSA = slave_address;                  // set slave address
  IE2 &= ~(UCB0TXIE + UCB0RXIE);              // no RX or TX interrupts
  __disable_interrupt();
  UCB0CTL1 |= UCTR + UCTXSTT + UCTXSTP;       // I2C TX, start condition
  while (UCB0CTL1 & UCTXSTP);                 // wait for STOP condition

  returnValue = !(UCB0STAT & UCNACKIFG);
  __enable_interrupt();
  IE2 = ie2_bak;                              // restore IE2
  UCB0I2CSA = slaveadr_bak;                   // restore old slave address
  UCB0I2CIE = ucb0i2cie;                      // restore old UCB0CTL1
  return returnValue;                         // return whether or not
                                              // a NACK occured
}

unsigned char TI_USCI_I2C_notready(){
  return (UCB0STAT & UCBBUSY);
}


#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{
  if (UCB0STAT & UCNACKIFG){            // send STOP if slave sends NACK
    UCB0CTL1 |= UCTXSTP;
    UCB0STAT &= ~UCNACKIFG;
  }

}


#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
  if (IFG2 & UCB0RXIFG)
  {
    if ( byteCtr == 0 )
    {
      UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
      *TI_receive_field = UCB0RXBUF;
      TI_receive_field++;
    }
    else
    {
      *TI_receive_field = UCB0RXBUF;
      TI_receive_field++;
      byteCtr--;
    }
  }
  else
  {
      //byte counter = 0, check for a no stop flag
      //no stop flag is used for sending a repeat start condition
      //
    if (byteCtr == 0)
    {
        if (!I2C_NO_STOP_FLAG)
            UCB0CTL1 |= UCTXSTP;    // I2C stop condition

        I2C_NO_STOP_FLAG = 0x00;    //clear the no stop always
        IFG2 &= ~UCB0TXIFG;         // Clear USCI_B0 TX int flag
    }
    else
    {
      UCB0TXBUF = *TI_transmit_field;
      TI_transmit_field++;
      byteCtr--;
    }
  }
}
