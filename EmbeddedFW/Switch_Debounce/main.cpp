/* Author: Cameron Weigel
 * Switch Debouncer project
*/
#include "mbed.h"

InterruptIn db_1 (p21);

InterruptIn db_2(p22);

DigitalOut sw_on (p20,1);

DigitalOut led_1 (p23);

Timer t_1, t_2;

int debouncer_rise = 30;
int debouncer_fall = 25;

int k = 0;

Serial pc(USBTX,USBRX);

void debounce_rise() {
	t_1.start();

	if (t_1.read_us() > debouncer_rise) {
		//Do ISR work
	}
	t_1.reset();



}

void debounce_fall() {
	t_2.start();

	if (t_2.read_us() > debouncer_fall) {
		//Do ISR work
		t_2.reset();
	}



}

int main() 
{
	t_1.start();
	t_2.start();

	db_1.rise(&debounce_rise);

	db_1.fall(&debounce_fall);

	}



;
