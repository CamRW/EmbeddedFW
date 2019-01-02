
#include "mbed.h"
#include "TextLCD.h"

const float TempSensitivity = 0.0625;	// degrees C/LSB (per TMP102 Data sheet)
const int temp_addr = 0x90;	// Default I2C address of TMP102
I2C tempsensor(p9,p10);


const float AccelSensitivity = 0.0039; // g/LSB
const int accel_write = 0xA6; // Write byte, chosen by grounding SD0
const int accel_read = 0xA7; // Read byte, chosen by grounding SD0
const char dataX0_reg = 0x32; // Register to start reading data


I2C accel(p9,p10);

Serial pc(USBTX,USBRX);

TextLCD lcd(p21, p22, p23, p24, p25, p26);


char config_t[3];
char temp_read[2];
float temp;

char dev_id;

char config_a[2];
char write_a[3];
char buffer[6];
int16_t accel_data[3];
float x, y, z;


int main() 
{
	pc.baud(115200);
	pc.printf("Starting TMP102 Test...\r\n\r\n");
	// Initialize TMP102
	config_t[0] = 0x01;		// set pointer register to 'config register' (Table 7 data sheet)
	config_t[1] = 0x60;		// config temperature measurements to 12-bit resolution (Table 10 data sheet)
	config_t[2] = 0xA0;		// configure temperature conversion rate to 4 Hz, AL to normal (Table 11 data sheet)
	tempsensor.write(temp_addr, config_t, 3);	// write 3 bytes to device at address 0x90
	// Start temperature measurments at a 4Hz rate
	config_t[0] = 0x00;		// set pointer register to 'data register' (Table 7 datasheet)
	tempsensor.write(temp_addr, config_t,1);		// send to pointer 'read temp'

	config_a[0] = 0x2C;
	config_a[1] = 0x0A;

	accel.write(accel_write, config_a,2);

	config_a[0] = 0x31; // ADXL data format register
	config_a[1] = 0x3B; // +/- 16g 0.003g LSB

	accel.write(accel_write, config_a,2);

	config_a[0] = 0x2D;
	config_a[1] = 0x08;

	accel.write(accel_write,config_a,2);


	while(1)
	{
		wait(1.);
		tempsensor.read(temp_addr, temp_read, 2);	// read the 2-byte temperature data
		// convert to 12-bit temp data (see Tables 8 & 9 in data sheet)
		temp = TempSensitivity * (((temp_read[0] << 8) + temp_read[1]) >> 4);
		pc.printf("Temp = %.2f deg C\r\n",temp);

		wait_ms(500);

		accel.write(accel_write, &dataX0_reg, 1);
		accel.read(accel_read, buffer, 6);

		accel_data[0] = buffer[1]<<8 | buffer[0]; 	// Combine MSB with LSB
		accel_data[1] = buffer[3]<<8 | buffer[2];
		accel_data[2] = buffer[5]<<8 | buffer[4];

		x = accel_data[0]*AccelSensitivity;	// x-axis acceleration in G's
		y = accel_data[1]*AccelSensitivity;	// y-axis acceleration in G's
		z = accel_data[2]*AccelSensitivity;	// z-axis acceleration in G's
		pc.printf("x:  %+4.2f g (%+4d), y:  %+4.2f g (%+4d), z:  %+4.2f g (%+4d)\r\n",x,accel_data[0],y,accel_data[1],z,accel_data[2]);




		lcd.cls();
		lcd.printf("x %+4.2f y %+4.2f z %4.2f T %+4.2f", x,y,z,temp);
		lcd.locate(0,0);


	}
}
