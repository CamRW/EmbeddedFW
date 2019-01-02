/* Author: Cameron Weigel
 * Timer/Ticker LED project
*/
#include "mbed.h"

DigitalOut t_1_led (p20,1);
DigitalOut t_2_led (p22,1);

DigitalOut tick_led (p16,1);

Timer t_1;
Timer t_2;

Ticker tick;


Serial pc(USBTX, USBRX);

void blink_led();

int main() 
{
	t_1.start();
	t_2.start();

	tick.attach(&blink_led,1.25);

	while(1) {
		t_1.read();
		t_2.read();


		if (t_1.read_ms() > 100) {
			t_1_led = !t_1_led;
			t_1.reset();
		}

		if (t_2.read_ms() > 500) {
			t_2_led = !t_2_led;
			t_2.reset();
		}




	}


};

void blink_led() {
	tick_led = !tick_led;
}
