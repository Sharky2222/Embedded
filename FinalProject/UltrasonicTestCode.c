#include <msp430g2553.h>

volatile long distance;

void ConfigureAdc_ultra(void);
void timer_setup(void);
void uart_init(void);
void ConfigClocks(void);
void port_init(void);

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    //void portInit(void);
    P1DIR = BIT4;           //Set P1.4 to output for ultrasonic sensor trigger pin
    P1DIR = BIT6;           //red led output
    P1SEL |= BIT4;
    P1OUT = 0x00;           //Clears output

    //void ConfigureAdc_ultra(void);
    ADC10CTL0 = ADC10SHT_3 | ADC10ON ;                //64 x ADC10CLKs
    ADC10CTL0 |= ENC| MSC;                            //converter Enable, Sampling/Conversion start, multiple sample/conversion operations
    ADC10CTL1 = INCH_5 + ADC10DIV_0 + CONSEQ_2;
    __delay_cycles(5);                                 //wait for adc Ref to settle

    //void timerSetup(void);

    TA0CTL = TASSEL_2 | ID_3 | MC_1 | TACLR; //SMCLK, divide by 8, up mode, reset TAR

    TA0CCR0 = 7499;        //Upper bound for the signal, period
    TA0CCR1 = 2;         //lower bound for the signal to define duty cycle

    TA0CCTL0 |= CCIE;       //Capture compare interrupt enable
    TA0CCTL1 |= CCIE;

    TA0CCTL0 &= ~CCIFG;     //Capture/compare interrupt flag when interrupt pending
    TA0CCTL1 &= ~CCIFG;

    ADC10CTL0 |= ENC + ADC10SC +MSC;       // Converter Enable, Sampling/conversion start
    ADC10CTL0 &= ~ADC10IFG;                // clear the flag

    _enable_interrupts();   //Enter LPM0

    while(1);
}



// Configure ADC Ultrasonic
void ConfigureAdc_ultra(void){

}

void start_Ultra(void){

}

void ConfigClocks(void){

}

void port_init(void){

}

void timer_setup(void){


}

#pragma vector = TIMER0_A0_VECTOR   //define the interrupt service vector
__interrupt void TA0_ISR (void){    //interrupt service routine

    P1OUT |= BIT4;          //Outputs PWM signal high

    distance = ADC10MEM / 58;                    // read the converted data into a variable

    if(distance > 30){
        P1OUT |= BIT6;
    }
    TA0CCTL0 &= ~CCIFG;     //Reset the interrupt flag
}




