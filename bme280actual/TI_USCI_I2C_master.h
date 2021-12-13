//the ti method for i2c config

#ifndef USCI_LIB
#define USCI_LIB

#define SDA_PIN BIT7                                  // msp430G2553 UCB0SDA pin, Check this
#define SCL_PIN BIT6                                  // msp430G2553 UCB0SCL pin, Check this

void TI_USCI_I2C_receiveinit(unsigned char slave_address, unsigned char prescale);
void TI_USCI_I2C_transmitinit(unsigned char slave_address, unsigned char prescale);


void TI_USCI_I2C_receive(unsigned char byteCount, unsigned char *field);
void TI_USCI_I2C_transmit(unsigned char byteCount, unsigned char *field);


void TI_USCI_I2C_transmit_nostop(unsigned char byteCount, unsigned char *field);



unsigned char TI_USCI_I2C_slave_present(unsigned char slave_address);
unsigned char TI_USCI_I2C_notready();


#endif
