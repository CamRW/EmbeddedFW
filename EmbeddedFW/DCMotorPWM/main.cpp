/* Author: Cameron Weigel
 * DC Motor PWM Project
*/
#include "mbed.h"

PwmOut motor_pin(p21);

DigitalIn start_motor(p11);

int main() 
{
	motor_pin.period(3);

    while(1) 
    {
    	start_motor.read();

    	if (start_motor == 1){

    		for (int i = 0; i<=10; i++){
    			motor_pin.write(i*.1);

    			wait(2);

    		}

    		for (int k = 5; k<0; k--) {
    			motor_pin.write(k*.2);

    			wait(2);

    		}

    	}

    	}


}
;
