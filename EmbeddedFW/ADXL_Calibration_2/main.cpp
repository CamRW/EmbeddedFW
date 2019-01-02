#include "mbed.h"
#include "TextLCD.h"
#include "ADXL345.h"
#include "IAP.h"


#define MEM_SIZE 256
#define TARGET_SECTOR 29

IAP iap;

I2C accel(p9,p10);

DigitalOut LED_INT(p13);
InterruptIn Accel_INT2(p21);


Serial pc(USBTX,USBRX);

const float AccelSensitivity = 0.0039; // g/LSB

char config_a[2]; // Config Array
char buffer[6]; // Raw data
int16_t data[3]; // Int data

// Function Prototypes

struct DATA_XYZ get_XYZ(I2C accel);

struct DATA_XYZ read_calibration();

void calibrate_XYZ(I2C accel, struct DATA_XYZ XYZ);

void TAP_ISR();

struct DATA_XYZ{
	float x;
	float y;
	float z;

};


int main() 
{
	struct DATA_XYZ XYZ;
	static char mem[ MEM_SIZE ];
	int r;
	char *p;

	XYZ = read_calibration;
	calibrate_XYZ(accel, XYZ);

	pc.baud(115200);

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

	while(1)
	{
		wait_ms(200);

		pc.printf(         "*** NXP Device-ID = 0x%08X ***\r\n\r\n", iap.read_ID() );
		DEVICE_ID sn = iap.read_serial();

		pc.printf("Device SN: 0x%08X 0x%08X 0x%08X 0x%08X\r\n\r\n",sn.DIN_0, sn.DIN_1, sn.DIN_2, sn.DIN_3);

		pc.printf("What would you like to do?\r\n");
		pc.printf("   (1) Blank Check All Sectors\r\n");
		pc.printf("   (2) Blank Check Sector 29\r\n");
		pc.printf("   (3) Erase Sector 29\r\n");
		pc.printf("   (4) Read 1st 256 bytes of Sector 29\r\n");
		pc.printf("   (5) Start XYZ calibration\r\n");

		pc.printf("   ?  ");

		while(pc.readable()==false)
			;
		char ch = pc.getc();
		pc.putc(ch);

		pc.printf("\r\n");

		switch (ch) {
		case '1':
			for (int i = 0; i <= 29; ++i) {
				if (i % 5 == 0)
					pc.printf("\r\n");
				r = iap.blank_check(i, i);
				pc.printf("Sector %02d: %02d\t", i, r);
			}
			break;
		case '2':
			int sector;
			sector = 29;
			pc.printf("\r\n");
			r = iap.blank_check(sector, sector);
			pc.printf("Sector %02d: %02d\r\n", sector, r);
			break;
		case '3':
			// Check to see if TARGET_SECTOR is BLANK
			r = iap.blank_check( TARGET_SECTOR, TARGET_SECTOR);

			// Erase Target Sector if NOT BLANK
			if (r == SECTOR_NOT_BLANK) {
				pc.printf("TARGET SECTOR is NOT BLANK!  Erasing...\r\n");
				iap.prepare( TARGET_SECTOR, TARGET_SECTOR);	// Always must prepare sector before erasing or writing
				r = iap.erase( TARGET_SECTOR, TARGET_SECTOR);
				pc.printf("erase result       = 0x%08X\r\n", r);
			}
			break;
		case '4':
			p = (char *) FLASH_SECTOR_29;
			for (int i = 0; i < 256; ++i) {
				if (i % 16 == 0)
					pc.printf("\r\n");
				pc.printf("%02X\t", *(p + i));
			}
			break;
		}



	}
}

void TAP_ISR() {
	LED_INT = 1;
	pc.printf("Single Tap Interrupt Occurred!!");
	wait(1.);
	LED_INT = 0;

}

void calibration_routine(I2C accel) {

	struct DATA_XYZ XYZ;
	struct DATA_XYZ XYZ_offset;

	static char mem[ MEM_SIZE ];
	int r;
	char *p;

	pc.printf("\r\n Starting XYZ calibration routine... \r\n");
	wait_ms(200);
	pc.printf("\r\n What would you like to do?\r\n");
	pc.printf("   (1) Calibrate X\r\n");
	pc.printf("   (2) Calibrate Y\r\n");
	pc.printf("   (3) Calibrate Z\r\n");
	pc.printf("   (4) Write calibration data to flash\r\n");
	while (pc.readable() == false)
		;
	char ch = pc.getc();
	pc.putc(ch);

	switch(ch) {
	case '1':

		pc.printf("\r\nPlace Accelerometer with X axis facing down\r\n");
		pc.printf("Press D when ready");

		while (pc.readable() == false)
			;
		char read_1 = pc.getc();
		pc.putc(read_1);

		if (read_1 == 'D' || read_1 == 'd') {

			int16_t accel_data[3];
			char buffer[6];
			const char dataX0_reg = 0x32; // Register to start reading data

			accel.write(ADXL345_I2C_WRITE, &dataX0_reg, 1);
			accel.read(ADXL345_I2C_READ, buffer, 6);

			accel_data[0] = buffer[1] << 8 | buffer[0]; // Combine MSB with LSB
			accel_data[1] = buffer[3] << 8 | buffer[2];
			accel_data[2] = buffer[5] << 8 | buffer[4];

			XYZ.x = accel_data[0] * AccelSensitivity; // x-axis acceleration in G's
			XYZ.y = accel_data[1] * AccelSensitivity; // y-axis acceleration in G's
			XYZ.z = accel_data[2] * AccelSensitivity;

			XYZ_offset.x = XYZ.x - (XYZ.x - 1);
		}
		break;
	case '2':
		pc.printf("\r\nPlace Accelerometer with Y axis facing down\r\n");
		pc.printf("Press D when ready");

		while (pc.readable() == false)
			;
		char read_2 = pc.getc();
		pc.putc(read_2);

		if (read_2 == 'D' || read_2 == 'd') {

			int16_t accel_data[3];
			char buffer[6];
			const char dataX0_reg = 0x32; // Register to start reading data

			accel.write(ADXL345_I2C_WRITE, &dataX0_reg, 1);
			accel.read(ADXL345_I2C_READ, buffer, 6);

			accel_data[0] = buffer[1] << 8 | buffer[0]; // Combine MSB with LSB
			accel_data[1] = buffer[3] << 8 | buffer[2];
			accel_data[2] = buffer[5] << 8 | buffer[4];

			XYZ.x = accel_data[0] * AccelSensitivity; // x-axis acceleration in G's
			XYZ.y = accel_data[1] * AccelSensitivity; // y-axis acceleration in G's
			XYZ.z = accel_data[2] * AccelSensitivity;

			XYZ_offset.y = XYZ.y - (XYZ.y - 1);
		}
		break;
	case '3':
		pc.printf("\r\nPlace Accelerometer with Z axis facing down\r\n");
		pc.printf("Press D when ready");

		while (pc.readable() == false)
			;
		char read_3 = pc.getc();
		pc.putc(read_3);

		if (read_3 == 'D' || read_3 == 'd') {

			int16_t accel_data[3];
			char buffer[6];
			const char dataX0_reg = 0x32; // Register to start reading data

			accel.write(ADXL345_I2C_WRITE, &dataX0_reg, 1);
			accel.read(ADXL345_I2C_READ, buffer, 6);

			accel_data[0] = buffer[1] << 8 | buffer[0]; // Combine MSB with LSB
			accel_data[1] = buffer[3] << 8 | buffer[2];
			accel_data[2] = buffer[5] << 8 | buffer[4];

			XYZ.x = accel_data[0] * AccelSensitivity; // x-axis acceleration in G's
			XYZ.y = accel_data[1] * AccelSensitivity; // y-axis acceleration in G's
			XYZ.z = accel_data[2] * AccelSensitivity;

			XYZ_offset.z = XYZ.z - (XYZ.z - 1);
		}
		break;
	case '4':

		// copy struct data into RAM mem buffer
		p = (char *) &XYZ_offset;

		memset(&mem[0], 0, sizeof(mem));// Set all elements of mem array to 0

		// Copy data struct into mem array
		for (uint i = 0; i < sizeof(XYZ_offset); ++i)
			mem[i] = *(p + i);

		iap.prepare( TARGET_SECTOR, TARGET_SECTOR);	// Always must prepare sector before erasing or writing
		r = iap.write(mem, sector_start_adress[ TARGET_SECTOR], MEM_SIZE);
		pc.printf(
				"copied: SRAM(0x%08X)->Flash(0x%08X) for %d bytes. (result=0x%08X)\r\n",
				mem, sector_start_adress[ TARGET_SECTOR], MEM_SIZE, r);

		// compare
		r = iap.compare(mem, sector_start_adress[ TARGET_SECTOR], MEM_SIZE);
		pc.printf("compare result     = \"%s\"\r\n",
				r ? "FAILED - Sector was probably not Blank before writing" : "OK");


	}

}

struct DATA_XYZ read_calibration() {
	int r;
	struct DATA_XYZ *XYZptr;  // Pointer to structure with format of data
	// Check to see if TARGET_SECTOR is BLANK
	r = iap.blank_check( TARGET_SECTOR, TARGET_SECTOR );
	if ( r == SECTOR_NOT_BLANK ) {
		XYZptr = (struct DATA_XYZ *) FLASH_SECTOR_29;
		pc.printf("\r\nDATA_A:\tx = %f\ty = %f\tz = %d\r\n",XYZptr->x,XYZptr->y,XYZptr->z);
		return *XYZptr;
	} else{
		pc.printf("\r\nTARGET SECTOR IS BLANK!  No data available\r\n");
		return *XYZptr;
}
}

void calibrate_XYZ(I2C accel, struct DATA_XYZ XYZ) {
	char data[2];

	//Write X axis
	data[0] = ADXL345_OFSX_REG;
	data[1] = XYZ.x;
	accel.write(ADXL345_I2C_WRITE, data, 2);

	//Write Y axis
	data[0] = ADXL345_OFSY_REG;
	data[1] = XYZ.y;
	accel.write(ADXL345_I2C_WRITE, data, 2);

	//Write Z axis
	data[0] = ADXL345_OFSZ_REG;
	data[1] = XYZ.z;
	accel.write(ADXL345_I2C_WRITE, data, 2);


}
