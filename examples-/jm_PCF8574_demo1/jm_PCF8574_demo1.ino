
#define __PROG__ "jm_PCF8574_demo1"

////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>

////////////////////////////////////////////////////////////////////////////////

#include <jm_PCF8574.h>

jm_PCF8574 PCF8574(0x3F);

////////////////////////////////////////////////////////////////////////////////

void setup()
{
	Serial.begin(115200);
	while (!Serial && millis()<3000); // wait for USB Serial ready
	Serial.print(F(__PROG__));
	Serial.print(F("..."));
	Serial.println();

	Wire.begin();

	PCF8574.begin();

	Serial.print(F("portRead()"));
	Serial.print(F("="));
	Serial.println(PCF8574.portRead(), HEX);

	PCF8574.portMode(OUTPUT);
	Serial.print(F("portWrite(0xF8)"));
	Serial.println(PCF8574.portWrite(0xF8), HEX);

	Serial.print(F("portRead()"));
	Serial.print(F("="));
	Serial.println(PCF8574.portRead(), HEX);

	Serial.print(F("read()"));
	Serial.print(F("="));
	Serial.println(PCF8574.read(), HEX);

	for (uint8_t pin=0; pin<8; pin++) {
		PCF8574.pinMode(pin, INPUT_PULLUP);
		Serial.print(F("digitalRead("));
		Serial.print(pin);
		Serial.print(F(")"));
		Serial.print(F("="));
		Serial.println(PCF8574.digitalRead(pin), HEX);
	}

	Serial.println();

	PCF8574.pinMode((uint8_t)3, OUTPUT);
	for (int i=0; i<10; i++) {
		//
		Serial.print(F("digitalWrite("));
		Serial.print((uint8_t)3);
		Serial.print(F(","));
		Serial.print(LOW);
		Serial.print(F(")"));
		Serial.println();
		for (int i=0; i<50; i++) PCF8574.wait((uint16_t)1000);
//		Serial.println(PCF8574.digitalWrite((uint8_t)3, LOW));
		PCF8574.digitalWrite((uint8_t)3, LOW);
		//
		Serial.print(F("digitalWrite("));
		Serial.print((uint8_t)3);
		Serial.print(F(","));
		Serial.print(HIGH);
		Serial.print(F(")"));
		Serial.println();
		for (int i=0; i<50; i++) PCF8574.wait((uint16_t)1000);
//		Serial.println(PCF8574.digitalWrite((uint8_t)3, HIGH));
		PCF8574.digitalWrite((uint8_t)3, HIGH);
	}

	Serial.println();
}

void loop()
{
}
