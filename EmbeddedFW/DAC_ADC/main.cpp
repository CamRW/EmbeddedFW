/* Author: Cameron Weigel
 * HEmbeddedLed project
*/
#include "mbed.h"

AnalogIn adc_in(p16);

AnalogOut dac_out(p18);

int x;


Serial pc(USBTX, USBRX);


int main() 
{
	dac_out.write(1.5/3.3);

	x = adc_in.read_u16();

	pc.printf("At 1.5 V %d\r\n",x);

	dac_out.write(2.5/3.3);

	x = adc_in.read_u16();

	pc.printf("At 2.5 V %d\r\n",x);


};
