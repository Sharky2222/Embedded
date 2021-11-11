#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    TACTL = TASSEL_2 | ID_3 | MC_1; //Set the configuration for timer_A control register

	P1DIR = BIT6;           //Set P1.6 to output
	P1SEL |= BIT6;          //Allows PWM output by setting the primary peripheral module for P1.6
	P1OUT = 0x00;           //Clears output

	TA0CCR0 = 62499;        //Upper bound for the signal, period
	TA0CCR1 = 6250;         //lower bound for the signal to define duty cycle
	TACCTL1 = OUTMOD_7;     //set/reset output



	while(1){
	    P1OUT ^=BIT6;       //Changes the output of P1OUT
	}

	return 0;
}

