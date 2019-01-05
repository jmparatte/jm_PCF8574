
#define __PROG__ "jm_PCF8574_blink"

////////////////////////////////////////////////////////////////////////////////

/*	This example blinks the backlight of a LCM2004A I2C LCD module.
	The LCM2004A module is commonly interfaced for I2C with a PCF8574A,
	optionnaly with a PCF8574.

	HD447800 datasheet:
	https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
	...

	LCM2004A datasheet:
	https://www.beta-estore.com/download/rk/RK-10290_410.pdf
	http://www.farnell.com/datasheets/50586.pdf
	...

	LCM2004A schematic:
	https://www.mpja.com/download/35054opdata.pdf
	...
*/

#define LCM2004A_I2C_ADR	((uint8_t) 0x3F)		// default PCF8574A I2C address
													// (alternative PCF8574 0x27)

#define LCM2004A_I2C_DB		((uint8_t) 0b11110000)	// Four high order data bus pins
#define LCM2004A_I2C_DB7	((uint8_t) 0b10000000)	//
#define LCM2004A_I2C_DB6	((uint8_t) 0b01000000)	//
#define LCM2004A_I2C_DB5	((uint8_t) 0b00100000)	//
#define LCM2004A_I2C_DB4	((uint8_t) 0b00010000)	//
#define LCM2004A_I2C_BL		((uint8_t) 0b00001000)	// LCD backlight
#define LCM2004A_I2C_E		((uint8_t) 0b00000100)	// Starts data read/write
#define LCM2004A_I2C_R_W	((uint8_t) 0b00000010)	// Selects Read/Write (1/0)
#define LCM2004A_I2C_RS		((uint8_t) 0b00000001)	// Selects Instruction/Data register (0/1)

#define LCM2004A_I2C_BL_PIN	((uint8_t) 3)			// P3 - LCD backlight pin number

////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>

////////////////////////////////////////////////////////////////////////////////

#include <jm_PCF8574.h>

jm_PCF8574 PCF8574(LCM2004A_I2C_ADR);

////////////////////////////////////////////////////////////////////////////////

void setup()
{
	Serial.begin(115200);
	while (!Serial && millis()<3000); // wait for USB Serial ready
	Serial.print(F(__PROG__));
	Serial.print(F("..."));
	Serial.println();
	Serial.flush();

	Wire.begin();

	PCF8574.begin();

	pinMode( LED_BUILTIN, OUTPUT );
	digitalWrite( LED_BUILTIN, LOW );

	PCF8574.pinMode( LCM2004A_I2C_BL_PIN, OUTPUT );
	PCF8574.digitalWrite( LCM2004A_I2C_BL_PIN, LOW );
}

void loop()
{
	digitalWrite( LED_BUILTIN, HIGH );
	PCF8574.digitalWrite( LCM2004A_I2C_BL_PIN, HIGH );
	delay(500);

	digitalWrite( LED_BUILTIN, LOW );
	PCF8574.digitalWrite( LCM2004A_I2C_BL_PIN, LOW );
	delay(500);
}
