
PROJECT TITLE:                      Lab05 
PURPOSE OF PROJECT:                 To generate a PWM signal at a period of 250ms and a duty cycle of 20%
VERSION or DATE:                    11/10/2021
AUTHORS:                            Nikolas Figueroa
USER INSTRUCTIONS:                  

	line 9: 	Stops the watchdog timer 
	line 10:	Configures the timer_A conrol register. TASSEL_2 selects the SMCLK clock, 
			ID_3 divides the clock frequency by 8, so 1MHz / 8  125000Hz. MC_1 selects the up mode and TACLR clears TAR.
	line 12:	Set P1.6 to an output.
	line 13: 	Clear any previous output.
	line 15:	Set TACCR0 to define the period of the signal.
	line 16:	Set TACCR1 to define the duty cycle.
	line 18-19: 	Set the capture/compare bit on for TA0CCTL0 and TA0CCTL1.
	line 21-22: 	Capture/compare interrupt flag when interrupt is pending. 
	line 24:	Waits for an interrupt to be triggered.
	line 28:	Defines the interrupt service vector.
	line 29:	Interrupt service routine.
	line 30: 	Sets the output of P1.6 to high.
	line 31: 	Resets the interrupt flag.
	line 34:	Defines the interrupt service vector. 
	line 35:	Interrupt service routine.
	line 36:	Sets the output of P1.6 to low.
	line 37:	Resets the interrupt flag.

Overview:
	The use of two timer vectors is being used to generate the PWM signal which is considered as a hardware implementation. One vector sets the signal to high on an interrupt 
	and the other sets the signal to low on an interrupt. The SMCLK clock is being used with a divider of 8. The TACCR0 is used to help set the period and the TACCR1 is used to help set the duty cycle.
	The capture compare interrupts are used to help generate the PWM signal to P1.6. 