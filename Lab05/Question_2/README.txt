
PROJECT TITLE:                      Lab05 
PURPOSE OF PROJECT:                 To generate a PWM signal at a period of 500ms and a duty cycle of 10%
VERSION or DATE:                    11/10/2021
AUTHORS:                            Nikolas Figueroa
USER INSTRUCTIONS:                  

	line 9: 	Stops the watchdog timer 
	line 10:	Setting the configuration of the timer_A control register. TASSEL_2 selects the SMCLK clock which runs at 
			1MHz. ID_3 divides the clock frequency by 8, so 1MHz / 8 = 125000Hz. MC_1 selects the up mode.
	line 12:	Sets BIT6 or P1.6 to an output.
	line 13:	Sets the primary peripheral module for P1.6 for P1SEL to allow PWM output.
	line 14:	Clears the previous output.
	line 16:	Sets the period or upper bound of the up signal. 
	line 17:	Sets the duty cycle or the lower bound of the signal.
	line 18:	Allows output to be reset/set when TAR reaches TACCR1 it is reset, and set when TAR reaches TACCR0.
	line 22-24:	Changes the output of P1OUT for P1.6 to output the PWM signal.