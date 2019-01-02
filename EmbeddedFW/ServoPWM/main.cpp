/* Author: Cameron Weigel
 * Servo PWM Project
*/
#include "mbed.h"

PwmOut servo_pin(p21);

Serial pc(USBTX, USBRX);

int range = 180;
int degrees = 20;

float pulse;


int main() 
{

    while(1) 
    {
    	servo_pin.pulsewidth(.001);



    	for (int i = 0; i<=9; i++){

    		pulse = .0002*i+.001;

    		servo_pin.pulsewidth(pulse);

    		pc.printf("Degrees:  %d%10s", i*20);
    		wait(3);

    	}


    }

}
;
