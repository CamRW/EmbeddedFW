/* Author: Cameron Weigel
 * HEmbeddedLed project
*/
#include "mbed.h"

AnalogIn pot_reader(p16);

AnalogOut led_pin(p18);

float voltage;
float prev_voltage = 0;

Serial pc(USBTX, USBRX);

float abs(float x, float y);

int main() 
{

    while(1) 
    {
    	voltage = pot_reader.read()*3.3;

    	wait(.5);

    	if (abs(voltage,prev_voltage) > .05){
        	led_pin.write(voltage);
        	pc.printf("\r\nVoltage:  %f\r\n", voltage);
        	prev_voltage = voltage;
    	}
    }

}

float abs(float x, float y) {
	if ((x-y) < 0) {
		return -(x-y);
	}

	else {
		return (x-y);
	}

}
;
