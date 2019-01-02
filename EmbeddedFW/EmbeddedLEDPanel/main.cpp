/* Author: Cameron Weigel
 * Embedded LED Panel project displaying 0x000 to 0xFFF
*/
#include "mbed.h"

BusOut ledbus(p30,p29,p8,p7,p6,p5,p28);

DigitalOut digit_one = p14;
DigitalOut digit_two = p15;
DigitalOut digit_three = p16;
DigitalOut digit_four = p17;


//DigitalOut pin30(p30,1);

int zero = 0x3F;
int one = 0x6;
int two = 0x5B;
int three = 0x4F;
int four = 0x66;
int five = 0x6D;
int six = 0x7C;
int seven = 0x7;
int eight = 0x7F;
int nine = 0x67;

int a = 0x77;
int b = 0x7C;
int c = 0x39;
int d = 0x5E;
int e = 0x79;
int f = 0x71;

int characters[] = {zero,one,two,three,four,five,six,seven,eight,nine,a,b,c,d,e,f};

int main() 
{

    while(1) 
    {
    	digit_one=1;
    	for (int i=0; i<16; i++)
    	{
    	ledbus = characters[i];
    	wait(.4);

    	digit_one= 0;
    	wait(.5);
    	}
    }
}
