#include "mbed.h"
#include "TextLCD.h"
#include "ADXL345.h"


I2C accel(p9,p10);

DigitalOut LED_INT(p13);
InterruptIn Accel_INT2(p5);

const char dataX0_reg = 0x32; // Register to start reading data


Serial pc(USBTX,USBRX);
TextLCD lcd(p21, p22, p23, p24, p25, p26);


const float AccelSensitivity = 0.0039; // g/LSB


char config_a[2]; // Config Array
char buffer[6]; // Raw data
int16_t data[3]; // Int data
float x, y, z; // Human-readable data in Gs

// Function Prototypes

void TAP_ISR();


int main() 
{
	pc.baud(115200);
	pc.printf("Starting ADXL345 Test...\r\n\r\n");
	// Initialize ADXL345


	// Start Configuration

	config_a[0] = ADXL345_BW_RATE_REG;
	config_a[1] = ADXL345_6HZ25;

	accel.write(ADXL345_I2C_WRITE, config_a,2);

	config_a[0] = ADXL345_DATA_FORMAT_REG; // ADXL data format register
	config_a[1] = 0x3B; // +/- 16g 0.003g LSB

	accel.write(ADXL345_I2C_WRITE, config_a,2);

	config_a[0] = ADXL345_POWER_CTL_REG; // Power ctl register
	config_a[1] = MeasurementMode; // Set Measurement mode

	accel.write(ADXL345_I2C_WRITE,config_a,2);


	// ------ Configure Interrupt ------

	// Disable interrupt enable for configuration

	config_a[0] = ADXL345_INT_ENABLE_REG;
	config_a[1] = 0x00; // Disable int enable

	accel.write(ADXL345_I2C_WRITE,config_a,2);

	char tx = ADXL345_INT_ENABLE_REG;
	char output;
	accel.write(ADXL345_I2C_WRITE,&tx ,1);
	accel.read(ADXL345_I2C_READ,&output ,1);

	pc.printf("INT ENABLE REG %d\r\n", output);

	// TAP AXES

	config_a[0] = ADXL345_TAP_AXES_REG;
	config_a[1] = 0x07;

	accel.write(ADXL345_I2C_WRITE,config_a,2);

	tx = ADXL345_TAP_AXES_REG;

	accel.write(ADXL345_I2C_WRITE,&tx ,1);
	accel.read(ADXL345_I2C_READ,&output ,1);

	pc.printf("TAP AXES REG %d\r\n", output);



	// 8G Thresh_Tap

	config_a[0] = ADXL345_THRESH_TAP_REG;
	config_a[1] = 0x10;

	accel.write(ADXL345_I2C_WRITE,config_a,2);


	tx = ADXL345_THRESH_TAP_REG;

	accel.write(ADXL345_I2C_WRITE,&tx ,1);
	accel.read(ADXL345_I2C_READ,&output ,1);

	pc.printf("THRESH TAP %d\r\n", output);

	// THresh ACT

	config_a[0] = ADXL345_THRESH_ACT_REG;
	config_a[1] = 0x10;

	accel.write(ADXL345_I2C_WRITE,config_a,2);


	tx = ADXL345_THRESH_ACT_REG;

	accel.write(ADXL345_I2C_WRITE,&tx ,1);
	accel.read(ADXL345_I2C_READ,&output ,1);

	pc.printf("THRESH ACT %d\r\n", output);





	// Single Tap Duration

	config_a[0] = ADXL345_DUR_REG;
	config_a[1] = 0x0F; // 128*625 us =  4 ms

	accel.write(ADXL345_I2C_WRITE,config_a,2);

	tx = ADXL345_DUR_REG;

	accel.write(ADXL345_I2C_WRITE,&tx ,1);
	accel.read(ADXL345_I2C_READ,&output ,1);

	pc.printf("DUR REG %d\r\n", output);

	// Disable double tap

	config_a[0] = ADXL345_LATENT_REG;
	config_a[1] = 0x00;

	accel.write(ADXL345_I2C_WRITE,config_a,2);

	tx = ADXL345_LATENT_REG;

	accel.write(ADXL345_I2C_WRITE,&tx ,1);
	accel.read(ADXL345_I2C_READ,&output ,1);

	pc.printf("LATENT REG %d\r\n", output);

	config_a[0] = ADXL345_WINDOW_REG;
	config_a[1] = 0x00;

	accel.write(ADXL345_I2C_WRITE, config_a, 2);

	tx = ADXL345_LATENT_REG;

	accel.write(ADXL345_I2C_WRITE, &tx, 1);
	accel.read(ADXL345_I2C_READ, &output, 1);

	pc.printf("WINDOW REG %d\r\n", output);


	// Map Interrupt

	config_a[0] = ADXL345_INT_MAP_REG;
	config_a[1] = 0x40; // Send SINGLE_TAP to INT2, rest to INT 1

	accel.write(ADXL345_I2C_WRITE,config_a,2);

	tx = ADXL345_INT_MAP_REG;

	accel.write(ADXL345_I2C_WRITE,&tx ,1);
	accel.read(ADXL345_I2C_READ,&output ,1);

	pc.printf("MAP REG %d\r\n", output);

	// Enable Interrupt


	config_a[0] = ADXL345_INT_ENABLE_REG;
	config_a[1] = 0x40; // Enable SINGLE_TAP Interrupt

	accel.write(ADXL345_I2C_WRITE,config_a,2);

	tx = ADXL345_INT_ENABLE_REG;

	accel.write(ADXL345_I2C_WRITE,&tx ,1);
	accel.read(ADXL345_I2C_READ,&output ,1);

	pc.printf("INT ENABLE REG %d\r\n", output);

	// ------ End Interrupt Configuration ------


	// End Configuration

	Accel_INT2.rise(TAP_ISR);

	while(1)
	{
		wait_ms(500);
		accel.write(ADXL345_I2C_WRITE, &dataX0_reg, 1);
		accel.read(ADXL345_I2C_READ, buffer, 6);

		data[0] = buffer[1] << 8 | buffer[0]; 	// Combine MSB with LSB
		data[1] = buffer[3] << 8 | buffer[2];
		data[2] = buffer[5] << 8 | buffer[4];

		x = data[0] * AccelSensitivity;	// x-axis acceleration in G's
		y = data[1] * AccelSensitivity;	// y-axis acceleration in G's
		z = data[2] * AccelSensitivity;	// z-axis acceleration in G's
		pc.printf(
				"x:  %+4.2f g (%+4d), y:  %+4.2f g (%+4d), z:  %+4.2f g (%+4d)\r\n",
				x, data[0], y, data[1], z, data[2]);

		lcd.cls();
		lcd.printf("x %+4.2f y %+4.2f z %4.2f", x, y, z);
		lcd.locate(0, 0);


	}
}

void TAP_ISR() {
	LED_INT = 1;
	pc.printf("Single Tap Interrupt Occurred!!");
	wait(1.);
	LED_INT = 0;

}
