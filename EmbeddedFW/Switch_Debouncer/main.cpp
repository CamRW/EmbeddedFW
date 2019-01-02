/* Author: Cameron Weigel
 * Switch Debouncer project
*/
#include "mbed.h"

InterruptIn db_1 (p21);

InterruptIn db_2(p22);

DigitalOut sw_on (p20,1);

Timer t_1;

int time_rise, time_fall;

int i=0;
int k=0;

Serial pc(USBTX,USBRX);

void counter_on() {
	t_1.start();
	i=i+1;
	t_1.stop();
	time_rise = time_rise + t_1.read_us();
	t_1.reset();

}

void counter_off() {
	t_1.start();
	k=k+1;
	t_1.stop();
	time_fall = time_fall + t_1.read_us();
	t_1.reset();
}

int main() 
{

	db_1.rise(&counter_on);

	db_1.fall(&counter_off);

	while(1){


		char c = pc.getc();

		if (c =='p') {

			pc.printf("\r\n %d bounces on fall \r\n", i);
			pc.printf("\r\n %d us of bouncing \r\n", time_rise);
			time_rise = 0;
			i = 0;
		}

		if (c == 'k') {
			pc.printf("\r\n %d bounces rise \r\n",k);
			pc.printf("\r\n %d us of bouncing \r\n", time_fall);
			time_fall = 0;
			k = 0;

		}

	}



};
