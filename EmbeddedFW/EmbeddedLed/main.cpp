/* Author: Cameron Weigel
 * HEmbeddedLed project
*/
#include "mbed.h"

DigitalOut ledpin(p29,0); //GPIO pin p29 will control LED


double orig_interval = .2; // 5 Hz


int main() 
{

    while(1) 
    {
    	ledpin = 1;
    	wait(orig_interval);
    	ledpin 0;
    	wait(orig_interval);
    }

}
;
