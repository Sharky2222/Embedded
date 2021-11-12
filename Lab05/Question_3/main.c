#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    TA0CTL = TASSEL_2 | ID_3 | MC_1 | TACLR; //SMCLK, divide by 8, up mode, reset TAR

    P1DIR = BIT6;           //Set P1.6 to output
    P1OUT = 0x00;           //Clears output

    TA0CCR0 = 31249;        //Upper bound for the signal, period
    TA0CCR1 = 6250;         //lower bound for the signal to define duty cycle

    TA0CCTL0 |= CCIE;       //Capture compare interrupt enable
    TA0CCTL1 |= CCIE;

    TA0CCTL0 &= ~CCIFG;     //Capture/compare interrupt flag when interrupt pending
    TA0CCTL1 &= ~CCIFG;

    _enable_interrupts();   //Enter LPM0

}

#pragma vector = TIMER0_A0_VECTOR   //define the interrupt service vector
__interrupt void TA0_ISR (void){    //interrupt service routine
    P1OUT |= BIT6;            //Outputs PWM signal high
    TA0CCTL0 &= ~CCIFG;     //Reset the interrupt flag
}

#pragma vector = TIMER0_A1_VECTOR   //define the interrupt service vector
__interrupt void TA1_ISR (void){    //interrupt service routine
    P1OUT &= ~BIT6;            //Outputs PWM signal low
    TA0CCTL1 &= ~CCIFG;     //Reset the interrupt flag
}
