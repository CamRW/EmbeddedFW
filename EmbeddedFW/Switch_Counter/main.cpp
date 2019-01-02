/* Author: Cameron Weigel
 * Switch Debouncer project
*/
#include "mbed.h"

InterruptIn db_1 (p21);

InterruptIn db_2(p22);

DigitalOut sw_on (p20,1);

DigitalOut led_1 (p23);

Timer t_1, t_2;

int debouncer_rise = 1000;
int debouncer_fall = 1000;

int k = 0;

Serial pc(USBTX,USBRX);

void debounce_rise() {

	if (t_1.read_us() > 2000) {
		k = k+1;

		if (k == 10) {
				led_1 = 1;
				k = 0;
			}
			else {
				led_1 = 0;
			}
		t_1.reset();
	}


}

void debounce_fall() {

	if (t_2.read_us() > 2000) {
		k = k+1;

		if (k == 10) {
			led_1 = 1;
			k = 0;
		}
		else {
			led_1 = 0;
		}
		t_2.reset();
	}


}

int main() 
{
	t_1.start();
	t_2.start();

	db_1.rise(&debounce_rise);

	db_1.fall(&debounce_fall);





};
