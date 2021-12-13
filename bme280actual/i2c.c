#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "TI_USCI_I2C_master.h"
#include "i2c.h"

//#define I2C_ADDRESS (uint8_t)0x00;
#define I2C_BUS_PRESCALE ((uint8_t)(0x28))//set up i2c speed

void i2c_init(uint8_t address7bit) //find the i2c device and initialize it
{

    _EINT(); //enable the interupts
    TI_USCI_I2C_transmitinit(address7bit,I2C_BUS_PRESCALE); //set up the slave

}

void i2c_write(uint8_t address7bit, uint8_t* data, uint8_t len) //write to i2c
{
    _EINT(); //enable intereupts
    TI_USCI_I2C_transmitinit(address7bit, I2C_BUS_PRESCALE); //set up the write and wait for i2c
      while ( TI_USCI_I2C_notready() );

      TI_USCI_I2C_transmit(len, data); //transmit your data
       while ( TI_USCI_I2C_notready() );
}

void i2c_read(uint8_t address7bit, uint8_t* data, uint8_t len) //read i2c data
{
    _EINT();//enable the interrupt

    TI_USCI_I2C_receiveinit(address7bit, I2C_BUS_PRESCALE);
      while ( TI_USCI_I2C_notready() ); //set up msp to recieve the data

      TI_USCI_I2C_receive(len, data);
       while ( TI_USCI_I2C_notready() ); //read the data and save to it the buffer

}

void i2c_readAddress(uint8_t address7bit, uint8_t address, uint8_t* data, uint8_t len) //read your i2c address, this is taken from TI examples
{
    _EINT();

    //setup i2c slave for writing - send the address
    //(7bits, not upshifted) and the prescale
    TI_USCI_I2C_transmitinit(address7bit, I2C_BUS_PRESCALE);
    while ( TI_USCI_I2C_notready() );


    //send the register address - no stop condition
//  TI_USCI_I2C_transmit_nostop(1, &address);
    TI_USCI_I2C_transmit(1, &address);

    //wait a bit
    while ( TI_USCI_I2C_notready() );

    //setup i2c as a receiver
    TI_USCI_I2C_receiveinit(address7bit, I2C_BUS_PRESCALE);

    //wait a bit
    while ( TI_USCI_I2C_notready() );

    //read len bytes into data
    TI_USCI_I2C_receive(len, data);

    //wait a bit
    while ( TI_USCI_I2C_notready() );

}


