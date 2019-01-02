/* Author: Cameron Weigel
 * Embedded Motor Project
*/
#include "mbed.h"

DigitalOut motor_pin(p26,1); //GPIO pin p5 will control the opening of the MOSFET and the starting of the motor
//InterruptIn switch_pin(P0_7); // GPIO pin p7 will control the switch which in turn flips the motor pin HIGH and LOW

/*
void switch_on(){
	motor_pin = 1;
}

void switch_off() {
	motor_pin = 0;
}

*/
int main() 
{
	//switch_pin.rise(&switch_on);
	//switch_pin.fall(&switch_off);

    while(1) 
    {
    	motor_pin = 1;

    }
}
