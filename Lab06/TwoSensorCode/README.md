PROJECT TITLE: Lab06

PURPOSE OF PROJECT: To process two analog signals at once into a digital signal using ADC for a photoresistor and moisture sensor.

DATE: 12/3/2021

AUTHORS: Nikolas Figueroa, Sanjay Thallam, Joseph Verdade

Overview: 
	
	To read analog sensor data coming from the photoresistor and moisture sensor,
	the ADC on the MSP430 was used with multiple configurations. A control bit was 
	used to read switch back and forth between the configurations of both sensors. 
	Each sensor had a different configuration with different INCH channels which
	made this nescessary. The ADC then converts the received signals of both sensors 
	and processes it into a meaningful variable that signifies how much water the
	plant has or how much light the plant has received. The data is sampled on each 
	clock cycle and provides a UART function which outputs the data to the console 
	future use. 

	
