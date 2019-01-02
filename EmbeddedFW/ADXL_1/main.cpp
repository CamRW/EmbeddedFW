#include "mbed.h"
#include "TextLCD.h"

const float Sensitivity = 0.0039;	// G's/LSB (per ADXL345 Datasheet)

SPI acc(p5,p6,p7);  // MOSI (SDA), MISO (SDO), SCL
DigitalOut cs(p13);

TextLCD lcd(p21, p22, p23, p24, p25, p26);

Serial pc(USBTX,USBRX);

char buffer[6];
int16_t data[3];
float x, y, z;

int main() 
{
	pc.baud(115200);
	pc.printf("Starting ADXL345 Test...\r\n\r\n");
	// Initialize ADXL345
	cs = 1;					// ADXL345 not selected
	acc.format(8,3);		// 8 bit data, SPI Mode 3
	acc.frequency(2000000);	// 2 MHz SPI clock rate

	cs = 0;					// Select the ADXL345
	acc.write(0x31);		// ADXL345 Data Format Register
	acc.write(0x0B);		// format +/-16g, 0.003g/LSB
	cs = 1;					// End of Transmission

	cs = 0;					// Start a new Transmission
	acc.write(0x2D);		// ADXL345 Power Control Register
	acc.write(0x08);		// Select measure mode
	cs = 1;					// End of Transmission

	while(1)
	{
		wait_ms(200);

		cs = 0;				// Start a transmission
		acc.write(0x80|0x40|0x32);	// RW bit high, MB bit high, plus address
		for(int i = 0;i <= 5; ++i)
		{
			buffer[i] = acc.write(0x00);	// Read back 6 data bytes
		}
		cs = 1; 			// End of Transmission

		// Interpret the raw data bytes into meaningful results
		data[0] = buffer[1]<<8 | buffer[0]; 	// Combine MSB with LSB
		data[1] = buffer[3]<<8 | buffer[2];
		data[2] = buffer[5]<<8 | buffer[4];

		x = data[0]*Sensitivity;	// x-axis acceleration in G's
		y = data[1]*Sensitivity;	// y-axis acceleration in G's
		z = data[2]*Sensitivity;	// z-axis acceleration in G's
		pc.printf("x:  %+4.2f g (%+4d), y:  %+4.2f g (%+4d), z:  %+4.2f g (%+4d)\r\n",x,data[0],y,data[1],z,data[2]);

		lcd.cls();
		lcd.printf("x %+4.2f y %+4.2f z %+4.2f",x,y,z);
		lcd.locate(0,0);



	}
}
