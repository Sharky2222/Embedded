PROJECT TITLE: Lab06

PURPOSE OF PROJECT: To process an analog signal into a digital signal using ADC for a photoresistor.

DATE: 12/3/2021

AUTHORS: Nikolas Figueroa, Sanjay Thallam, Joseph Verdade

Overview: 
	
	Reads analog sensor data coming from the photoresistor by using the ADC on the MSP430. The ADC converts the signal to a digital signal which is then processed 
	in the variable lightLevel which determines a meaningful value of brightness based on the ranges of raw sensor data. The data is sampled by the photoresistor on
	each clock cycle. This code also provides a UART function which outputs the data to the console for future use. 
	
	