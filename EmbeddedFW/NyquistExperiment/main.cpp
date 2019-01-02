/* Author: Cameron Weigel
 * Nyquist project
*/
#include "mbed.h"

AnalogIn PWM_reader(p18);

PwmOut PWM_out(p21);

Serial pc(USBTX, USBRX);


float volt_readings[40*sizeof(float)];

int main() 
{
	PWM_out.period(.000002);


    	for (int i = 1; i<40; i++){
    		PWM_out.write(.5);
    		volt_readings[i]= PWM_reader.read();


    	}

    	pc.printf("\r\n Begin Volt Data \r\n");

    	for (int i=1; i<40; i++) {
    		pc.printf("%f\r\n", volt_readings[i]);
    	}

    	pc.printf("\r\n End Volt Data \r\n");

}
;
