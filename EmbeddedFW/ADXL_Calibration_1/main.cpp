#include "mbed.h"
#include "TextLCD.h"
#include "ADXL345.h"


I2C accel(p9,p10);

DigitalOut LED_INT(p13);
InterruptIn Accel_INT2(p21);


Serial pc(USBTX,USBRX);

const float AccelSensitivity = 0.0039; // g/LSB


char config_a[2]; // Config Array
char buffer[6]; // Raw data
int16_t data[3]; // Int data


struct DATA_XYZ {
	float x;
	float y;
	float z;
};


struct DATA_XYZ xyz;

// Function Prototypes

struct DATA_XYZ get_XYZ(I2C accel);


int main() 
{
	pc.baud(115200);
	pc.printf("Starting ADXL345 Test...\r\n\r\n");
	// Initialize ADXL345


	// Start Configuration

	config_a[0] = ADXL345_BW_RATE_REG;
	config_a[1] = ADXL345_100HZ;

	accel.write(ADXL345_I2C_WRITE, config_a,2);

	config_a[0] = ADXL345_DATA_FORMAT_REG; // ADXL data format register
	config_a[1] = 0x3B; // +/- 16g 0.003g LSB

	accel.write(ADXL345_I2C_WRITE, config_a,2);

	config_a[0] = ADXL345_POWER_CTL_REG; // Power ctl register
	config_a[1] = MeasurementMode; // Set Measurement mode

	accel.write(ADXL345_I2C_WRITE,config_a,2);


	// End Configuration

	char c;

	while(1)
	{
		wait_ms(200);
		c = pc.getc();
		xyz = get_XYZ(accel);


		if (c=='x') {
			pc.printf("\r\nCurrent X = %4.2f\r\n", xyz.x);
			float tmp_x;
			tmp_x = 0-xyz.x;

			int offset = tmp_x/.0156;

			float offset_mg_x = offset*15.6;

			char reg_offset_x = 0x00 + offset;

			pc.printf("X offset should be set by %4.2f\r\n", offset_mg_x);

			pc.printf("Writing %4.2f mg offset to X register\r\n", offset_mg_x);

			config_a[0] = ADXL345_OFSX_REG;
			config_a[1] = reg_offset_x;

			accel.write(ADXL345_I2C_WRITE,config_a,2);


		}

		if (c=='y') {
			pc.printf("\r\nCurrent Y = %4.2f\r\n", xyz.y);
			float tmp_y;
			tmp_y = 0-xyz.y;

			int offset = tmp_y/.0156;
			float offset_mg_y = offset*15.6;

			char reg_offset_y = 0x00 + offset;

			pc.printf("Y offset should be set by %4.2f\r\n", offset_mg_y);

			pc.printf("Writing %4.2f mg offset to Y register\r\n", offset_mg_y);

			config_a[0] = ADXL345_OFSY_REG;
			config_a[1] = reg_offset_y;

			accel.write(ADXL345_I2C_WRITE,config_a,2);
		}

		if (c=='z') {
			pc.printf("\r\nCurrent Z = %4.2f\r\n", xyz.z);
			float tmp_z;
			tmp_z = 1-xyz.z;

			int offset = tmp_z/.0156;
			float offset_mg_z = offset*15.6;

			char reg_offset_z = 0x00 + offset;

			pc.printf("Z offset should be set by %4.2f\r\n", offset_mg_z);

			pc.printf("Writing %4.2f mg offset to Z register\r\n", offset_mg_z);

			config_a[0] = ADXL345_OFSZ_REG;
			config_a[1] = reg_offset_z;

			accel.write(ADXL345_I2C_WRITE,config_a,2);
		}


	}
}


struct DATA_XYZ get_XYZ(I2C accel) {

	struct DATA_XYZ data_xyz;
	int16_t accel_data[3];
	char buffer[6];
	const char dataX0_reg = 0x32; // Register to start reading data


	accel.write(ADXL345_I2C_WRITE, &dataX0_reg, 1);
	accel.read(ADXL345_I2C_READ, buffer, 6);

	accel_data[0] = buffer[1]<<8 | buffer[0]; 	// Combine MSB with LSB
	accel_data[1] = buffer[3]<<8 | buffer[2];
	accel_data[2] = buffer[5]<<8 | buffer[4];

	data_xyz.x = accel_data[0]*AccelSensitivity;	// x-axis acceleration in G's
	data_xyz.y = accel_data[1]*AccelSensitivity;	// y-axis acceleration in G's
	data_xyz.z = accel_data[2]*AccelSensitivity;	// z-axis acceleration in G's

	return data_xyz;


}
