#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "TI_USCI_I2C_master.h"
#include "i2c.h"
#include "BME280.h"

volatile float temperature[5]={0x00};
volatile float humidity[5]={0x00};
volatile float pressure[5]={0x00};
volatile long light_Raw;                 //Analog input from photoresistor
volatile long lightLevel;                //Digital processed light input
volatile long water_Raw;                 //Analog input from moisture sensor
volatile long waterLevel;                //Digital processed moisture input
volatile long control = 0;               //Controls which ADC configurations to use

char result[100];                        //Used for displaying UART to console

void ConfigureAdc_water(void);
void ConfigureAdc_light(void);

void uart_init(void);
void ConfigClocks(void);
void port_init();
void strreverse(char* begin, char* end);
void itoa(int value, char* str, int base);
/**
 * main.c
 */

static volatile int TimeDelay; //this will state the delay value

#define greenled BIT0
int main(void)

{

    int pressure = 0;
    int temperature = 0;
    int humidity = 0;

    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    port_init();
    ConfigClocks();
    uart_init();
    i2c_init(0x77);
    BME280_init();

    _delay_cycles(5);                    // Wait for ADC Ref to settle

    while(1){

        if(control == 0){                //Control selects light sensor

            ConfigureAdc_light();
            ADC10CTL0 |= ENC + ADC10SC +MSC;                   // Converter Enable, Sampling/conversion start
            while((ADC10CTL0 & ADC10IFG) == 0);                // check the Flag, while its low just wait
            _delay_cycles(15000000);                            // delay for about 1 second to allow time to read data
            light_Raw = ADC10MEM;                              // read the converted data into a variable
            ADC10CTL0 &= ~ADC10IFG;                            // clear the flag
            //itoa((light_Raw),result,10);                       // format the light_Raw data into a string to be read on console


            if(light_Raw <= 250)  lightLevel = 0;                           //Dark out
            else if(light_Raw >= 250 && light_Raw < 500) lightLevel = 1;    //Little Sun
            else if(light_Raw >= 500 && light_Raw < 700 ) lightLevel = 2;   //Partly Sunny
            else if(light_Raw >= 800) lightLevel = 3;                       //Sunny Day

            itoa((lightLevel),result,10);
            int acount =0;

            while(result[acount]!='\0')
            {
                while((IFG2 & UCA0TXIFG)==0);                  //Wait Until the UART transmitter is ready
                UCA0TXBUF = result[acount++];                 //Transmit the received data.

            }
            //lightLevel = lightLevel;   //for graphing
            UCA0TXBUF = 'l';
            P2OUT ^= BIT1;
            control = 1;    //change the control variable to go to the next sensor

        }
        else if (control == 1){

            ConfigureAdc_water();
            ADC10CTL0 |= ENC + ADC10SC +MSC;       // Converter Enable, Sampling/conversion start
            while((ADC10CTL0 & ADC10IFG) == 0);    // check the Flag, while its low just wait
            _delay_cycles(15000000);                // delay for about 1 second to allow time to read data
            water_Raw = ADC10MEM;                  // read the converted data into a variable
            ADC10CTL0 &= ~ADC10IFG;                // clear the flag
            //itoa((water_Raw),result,10);           // format the water_Raw data into a string to be read on console


            if(water_Raw < 200) waterLevel = 0;                             //Dry
            else if(water_Raw >= 200 && water_Raw < 350) waterLevel = 1;    //little moisture
            else if(water_Raw >= 350 && water_Raw < 500) waterLevel = 2;    //Moist
            else if(water_Raw >= 500) waterLevel = 3;                       //Saturated

            itoa((waterLevel),result,10);
            int acount =0;

            while(result[acount]!='\0')
            {
                while((IFG2 & UCA0TXIFG)==0);                  //Wait Unitl the UART transmitter is ready
                UCA0TXBUF = result[acount++];                 //Transmit the received data.

            }
            //waterLevel = waterLevel;     //for graphing
            UCA0TXBUF = 'm';
            control = 2;    //change the control variable to go to the previous sensor
        }

        else if(control == 2)//temp
        {

            while((ADC10CTL0 & ADC10IFG) == 0);
            ADC10CTL0 &= ~ADC10IFG;
            BME280_Data bmeData = BME280_read();
            _delay_cycles(15000000);
            temperature = bmeData.cTemperatureFInt;
            int hundreds = temperature/100;
            int tens = temperature/10;
            int ones = temperature%10;
            int acount =0;

            //printf("%d", temperature);
            if(hundreds > 0){
                itoa((hundreds),result,10);

                while(result[acount]!='\0')
                {
                    while((IFG2 & UCA0TXIFG)==0);                  //Wait Unitl the UART transmitter is ready
                    UCA0TXBUF = result[acount++] ;                 //Transmit the received data.

                }
            }
            itoa((tens),result,10);

            acount =0;
            while(result[acount]!='\0')
            {
                while((IFG2 & UCA0TXIFG)==0);                  //Wait Unitl the UART transmitter is ready
                UCA0TXBUF = result[acount++] ;                 //Transmit the received data.

            }
            itoa((ones),result,10);

            acount =0;
            while(result[acount]!='\0')
            {
                while((IFG2 & UCA0TXIFG)==0);                  //Wait Unitl the UART transmitter is ready
                UCA0TXBUF = result[acount++] ;                 //Transmit the received data.

            }
            UCA0TXBUF='t';



            control = 3;

        }

        else if(control == 3)//humidiity
        {

            while((ADC10CTL0 & ADC10IFG) == 0);
            BME280_Data bmeData = BME280_read();
            _delay_cycles(15000000);
            humidity = bmeData.cHumidityInt;

            int hundreds = humidity/100;
            int tens = humidity / 10;
            int ones = humidity %10;
            int acount =0;
            if(hundreds > 0){
                itoa((hundreds),result,10);
                while(result[acount]!='\0')
                {
                    while((IFG2 & UCA0TXIFG)==0);                  //Wait Unitl the UART transmitter is ready
                    UCA0TXBUF = result[acount++] ;                 //Transmit the received data.

                }
            }

            itoa((tens),result,10);

            acount =0;
            while(result[acount]!='\0')
            {
                while((IFG2 & UCA0TXIFG)==0);                  //Wait Unitl the UART transmitter is ready
                UCA0TXBUF = result[acount++] ;                 //Transmit the received data.

            }
            itoa((ones),result,10);

            acount =0;
            while(result[acount]!='\0')
            {
                while((IFG2 & UCA0TXIFG)==0);                  //Wait Unitl the UART transmitter is ready
                UCA0TXBUF = result[acount++] ;                 //Transmit the received data.

            }
            UCA0TXBUF='h';



            control = 4;
        }

        else if(control == 4)//pressure
        {

            while((ADC10CTL0 & ADC10IFG) == 0);
            BME280_Data bmeData = BME280_read();
            _delay_cycles(15000000);
            pressure = bmeData.cPressureInt/100;
            //printf("%d", pressure);


            int acount =0;
            int hundreds = pressure/100;
            int tens = pressure / 10;
            int ones = (pressure) %10;
            itoa((pressure),result,10);


            while(result[acount]!='\0')
            {
                while((IFG2 & UCA0TXIFG)==0);                  //Wait Unitl the UART transmitter is ready
                UCA0TXBUF = result[acount++] ;                 //Transmit the received data.

            }
            /*
            itoa((tens),result,10);

            acount =0;
            while(result[acount]!='\0')
            {
                while((IFG2 & UCA0TXIFG)==0);                  //Wait Unitl the UART transmitter is ready
                UCA0TXBUF = result[acount++] ;                 //Transmit the received data.

            }
            itoa((ones),result,10);

            acount =0;
            while(result[acount]!='\0')
            {
                while((IFG2 & UCA0TXIFG)==0);                  //Wait Unitl the UART transmitter is ready
                UCA0TXBUF = result[acount++] ;                 //Transmit the received data.

            }*/
            UCA0TXBUF='p';



            control = 0;
        }

    }

    return 0;
}

// Configure ADC moisture sensor
void ConfigureAdc_water(){
    ADC10CTL0 &= ~ENC;                                   //allows the channel to be switched
    ADC10CTL1 = INCH_0 + ADC10DIV_0 + CONSEQ_2;          //Resets all INCH bits to 0000 which is channel p1.0
    ADC10CTL0 = SREF_1 | ADC10SHT_3 | REFON | ADC10ON ;  //Vref+, Vss, 64 ATD clocks per sample, internal references, turn ADCON
    __delay_cycles(5);                                   //wait for adc Ref to settle
    ADC10CTL0 |= ENC| MSC;                               //converter Enable, Sampling/Conversion start, multiple sample/conversion operations
}

// Configure ADC photoresistor
void ConfigureAdc_light(){
    ADC10CTL0 &= ~ENC;                                   //allows the channel to be switched
    ADC10CTL1 = INCH_3 + ADC10DIV_0 + CONSEQ_2;          //INCH channel is set to p1.3
    ADC10CTL0 = SREF_1 | ADC10SHT_3 | REFON | ADC10ON ;  //Vref+, Vss, 64 ATD clocks per sample, internal references, turn ADCON
    __delay_cycles(5);                                   //wait for adc Ref to settle
    ADC10CTL0 |= ENC| MSC;                               //converter Enable, Sampling/Conversion start, multiple sample/conversion operations
}

void uart_init(void){
    UCA0CTL1 |= UCSWRST;                     //Disable the UART state machine
    UCA0CTL1 |= UCSSEL_3;                    //Select SMCLK as the baud rate generator source
    UCA0BR1 = 0;
    UCA0BR0 = 9;                           //Produce a 115200 Baud UART rate
    UCA0MCTL |= 0x10;
    UCA0CTL1 &= ~UCSWRST;                    //Enable the UART state naching
    IE2 |= UCA0RXIE;                         //Enable the UART receiver Interrupt
}

void ConfigClocks(void)
{

    BCSCTL1 = CALBC1_1MHZ;                     // Set range
    DCOCTL = CALDCO_1MHZ;                      // Set DCO step + modulation
    BCSCTL3 |= LFXT1S_2;                       // LFXT1 = VLO
    IFG1 &= ~OFIFG;                            // Clear OSCFault flag
    BCSCTL2 = 0;                               // MCLK = DCO = SMCLK
}


void strreverse(char* begin, char* end)      // Function to reverse the order of the ASCII char array elements
{
    char aux;
    while(end>begin)
        aux=*end, *end--=*begin, *begin++=aux;
}

void itoa(int value, char* str, int base) {  //Function to convert the signed int to an ASCII char array

    static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* wstr=str;
    int sign;

    // Validate that base is between 2 and 35 (inlcusive)
    if (base<2 || base>35){
        *wstr='\0';
        return;
    }

    // Get magnitude and the value
    sign=value;
    if (sign < 0)
        value = -value;

    do // Perform integer-to-string conversion.
        *wstr++ = num[value%base]; //create the next number in converse by taking the modolus
    while(value/=base);  // stop when you get  a 0 for the quotient

    if(sign<0) //attach sign character, if needed
        *wstr++='-';
    *wstr='\0'; //Attach a null character at end of char array. The string is in revers order at this point
    strreverse(str,wstr-1); // Reverse string

}

void port_init(){
    P1OUT = 0x00;
    P1SEL |= BIT1 + BIT2;            // select non-GPIO  usage for Pins 1 and 2
    P1SEL2 |= BIT1 + BIT2;           // Select UART usage of Pins 1 and 2
}


