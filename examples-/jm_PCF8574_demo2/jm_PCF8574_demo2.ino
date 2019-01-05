
#define __PROG__ "jm_PCF8574_demo2"

////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>

////////////////////////////////////////////////////////////////////////////////

#include <jm_PCF8574.h>

jm_PCF8574 PCF8574(0x3F);

////////////////////////////////////////////////////////////////////////////////

// Entry mode set
#define HD44780U_COMMAND_ENTRY	((uint8_t) 0b00000100)
#define HD44780U_C_PARAM_I_D	((uint8_t) 0b00000010)
#define HD44780U_C_PARAM_S		((uint8_t) 0b00000001)

class jm_LCM2004 : public Print //Stream
{
private:
	bool _rd;
	bool _rs;

	bool send4bit(uint8_t send4bit, bool rs);
	bool command(uint8_t command);

protected:

public:
	jm_LCM2004();

	operator bool();

	bool clear();
	bool home();
	bool entry(uint8_t i_d=HD44780U_C_PARAM_I_D, uint8_t s=0);

	bool begin();

//	virtual int available() {return 0;}
//	virtual int peek() {return 0;}
//	virtual int read() {return 0;}

	virtual size_t write(uint8_t value);
	inline size_t write(unsigned long n) {return write((uint8_t) n);}
	inline size_t write(long n) {return write((uint8_t) n);}
	inline size_t write(unsigned int n) {return write((uint8_t) n);}
	inline size_t write(int n) {return write((uint8_t) n);}
	using Print::write;
};

jm_LCM2004::jm_LCM2004() :
	_rd(false),
	_rs(false)
{
}

jm_LCM2004::operator bool()
{
	return true;
}

bool jm_LCM2004::send4bit(uint8_t send4bit, bool rs)
{
	uint8_t s[] = {0b00001000,0b00001100,0b00001000}; // set/clr Backlight|RD|RS, pulse E
	for (size_t i=0; i<sizeof(s); i++) s[i] |= (send4bit | (rs ? 0b00000001 : 0));
	return (PCF8574.write((const uint8_t *) &s, sizeof(s)) == sizeof(s));
}

bool jm_LCM2004::command(uint8_t command)
{
	if (!send4bit(command & 0xF0, false)) return false;
	return send4bit((uint8_t) (command << 4), false);
}

// ---------------------------------------------------------------------------

// Clear display
// =============
//
// Clears entire display and sets DDRAM address 0 in address counter.
//
bool jm_LCM2004::clear()
{
	if (!command(0b00000001)) return false;
//	delayMicroseconds(37); // ?
	return true;
}

// Return home
// ===========
//
// Sets DDRAM address 0 in address counter.
// Also returns display from being shifted to original position.
// DDRAM contents remain unchanged.
//
bool jm_LCM2004::home()
{
	if (!command(0b00000010)) return false;
	delayMicroseconds(1520);
	return true;
}

bool jm_LCM2004::entry(uint8_t i_d, uint8_t s)
{
	if (!command(HD44780U_COMMAND_ENTRY | i_d | s)) return false;
	delayMicroseconds(37);
	return true;
}

// ---------------------------------------------------------------------------

bool jm_LCM2004::begin()
{
	// Software reset after Power On...

	delayMicroseconds(40000);

	if (!send4bit(0b00110000, false)) return false;

	delayMicroseconds(4100);

	if (!send4bit(0b00110000, false)) return false;

//	delayMicroseconds(37);

	if (!send4bit(0b00110000, false)) return false;

//	delayMicroseconds(37);

	if (!send4bit(0b00100000, false)) return false;

//	delayMicroseconds(37);

	// Set display lines and character font...

	if (!command(0b00101000)) return false;

//	delayMicroseconds(100);





	// Display off...

	if (!command(0b00001000)) return false;

	if (!clear()) return false;
	if (!entry()) return false;

	// Display on...

	if (!command(0b00001111)) return false;

	return true;
}

size_t jm_LCM2004::write(uint8_t value)
{
	if (!send4bit(value & 0xF0, true)) return 0;
	if (!send4bit((uint8_t) (value << 4), true)) return 0;

	return 1;
}

jm_LCM2004 LCM2004;

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
/*
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
*/

	LCM2004.begin();

	LCM2004.write('1');
	LCM2004.write('2');
	LCM2004.write('3');
	LCM2004.write('4');

	LCM2004.print("qwertz");

	LCM2004.write("xyz");
}

void loop()
{
}
