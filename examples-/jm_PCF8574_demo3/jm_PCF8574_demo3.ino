
#define __PROG__ "jm_PCF8574_demo3"

////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>

////////////////////////////////////////////////////////////////////////////////

#include <jm_PCF8574.h>

jm_PCF8574 PCF8574(0x3F);

////////////////////////////////////////////////////////////////////////////////

// Clear display
// =============
// Clears entire display and sets DDRAM address 0 in address counter.
//
// Execution time: 1520us ???
//
#define HD44780U_CLEAR		((uint8_t) 0b00000001)

// Return home
// ===========
//
// Sets DDRAM address 0 in address counter.
// Also returns display from being shifted to original position.
// DDRAM contents remain unchanged.
//
// Execution time: 1520us
//
#define HD44780U_HOME		((uint8_t) 0b00000010)

// Entry mode set
// ==============
//
// Sets cursor mode direction and specifies display shift.
// These operations are performed during data write and read.
//
// I/D = 1: Increment
// I/D = 0: Decrement
// S   = 1: Accompanies display shift
//
// Execution time: 37us
//
#define HD44780U_ENTRY		((uint8_t) 0b00000100)
#define HD44780U_ENTRY_I_D	((uint8_t) 0b00000010)
#define HD44780U_ENTRY_S	((uint8_t) 0b00000001)

// Display on/off control
// ======================
//
// Sets entire display (D) on/off, cursor on/off (C),
// and blinking of cursor position character (B).
//
// Execution time: 37us
//
#define HD44780U_DISPLAY	((uint8_t) 0b00001000)
#define HD44780U_DISPLAY_D	((uint8_t) 0b00000100)
#define HD44780U_DISPLAY_C	((uint8_t) 0b00000010)
#define HD44780U_DISPLAY_B	((uint8_t) 0b00000001)

// Cursor or display shift
// =======================
//
// Moves cursor and shifts display without changing DDRAM contents.
//
// S/C = 1: Display shift
// S/C = 0: Cursor move
// R/L = 1: Shift to the right
// R/L = 0: Shift to the left
//
// Execution time: 37us
//
#define HD44780U_CURSOR		((uint8_t) 0b00010000)
#define HD44780U_CURSOR_S_C	((uint8_t) 0b00001000)
#define HD44780U_CURSOR_R_L	((uint8_t) 0b00000100)

// Function set
// ============
//
// Sets interface data length (DL), number of data lines (N), and character font (F).
//
// DL  = 1: 8 bits, 0: 4 bits
// N   = 1: 2 lines, 0: 1 line
// F   = 1: 5x10 dots, 0: 5x8 dots
//
// Execution time: 37us
//
#define HD44780U_SET		((uint8_t) 0b00100000)
#define HD44780U_SET_DL		((uint8_t) 0b00010000)
#define HD44780U_SET_N		((uint8_t) 0b00001000)
#define HD44780U_SET_F		((uint8_t) 0b00000100)

// Set CGRAM address
// =================
//
// Sets CGRAM address.
// CGRAM data is sent and received after this setting.
//
// Execution time: 37us
//
#define HD44780U_CGRAM		((uint8_t) 0b01000000)
#define HD44780U_CGRAM_ACG	((uint8_t) 0b00111111)

// Set DDRAM address
// =================
//
// Sets DDRAM address.
// DDRAM data is sent and received after this setting.
//
// Execution time: 37us
//
#define HD44780U_DDRAM		((uint8_t) 0b10000000)
#define HD44780U_DDRAM_ADD	((uint8_t) 0b01111111)

// Read busy flag & address
// ========================
//
// Reads busy flag (BF) indicating internal operation is being performed
// and reads address counter contents.
// BF  = 1: Internally operating
// BL  = 0: Instructions acceptable
// AC  = Address counter used for both DD and CGRAM addresses
//
// Execution time: 0us
//
#define HD44780U_READ		((uint8_t) 0b00000000)
#define HD44780U_READ_BF	((uint8_t) 0b10000000)
#define HD44780U_READ_AC	((uint8_t) 0b01111111)

////////////////////////////////////////////////////////////////////////////////

class jm_LCM2004 : public Print
{
private:
	bool _rd;
	bool _rs;

protected:

public:
	jm_LCM2004();

	operator bool();

	bool send4bit(uint8_t send4bit, bool rs, uint16_t us);
	bool command(uint8_t command, uint16_t us);

	int rcve4bit();
	int state();

	bool clear();
	bool home();
	bool entry(uint8_t i_d=HD44780U_ENTRY_I_D, uint8_t s=0);
	bool display(bool D=true, bool C=false, bool B=false);
	bool cursor(bool S_C=false, bool R_L=true);
	bool set(bool DL=false, bool N=true, bool F=false); // 4 bits, 2 lines, 5x8 dots
	bool cgram(uint8_t ACG=0);
	bool ddram(uint8_t ADD=0);

	bool begin();

	virtual size_t write(uint8_t value);
	inline size_t write(unsigned long n) {return write((uint8_t) n);}
	inline size_t write(long n) {return write((uint8_t) n);}
	inline size_t write(unsigned int n) {return write((uint8_t) n);}
	inline size_t write(int n) {return write((uint8_t) n);}
	using Print::write;

	bool set_cursor(int col, int row);
};

// ---------------------------------------------------------------------------

jm_LCM2004::jm_LCM2004() :
	_rd(false),
	_rs(false)
{
}

jm_LCM2004::operator bool()
{
	return true;
}

// ---------------------------------------------------------------------------

bool jm_LCM2004::send4bit(uint8_t send4bit, bool rs, uint16_t us)
{
	uint8_t s[] = {0b00001000,0b00001100,0b00001000}; // set/clr Backlight|RD|RS, pulse E
	for (size_t i=0; i<sizeof(s); i++) s[i] |= (send4bit | (rs ? 0b00000001 : 0));
	if (PCF8574.write((const uint8_t *) &s, sizeof(s)) != sizeof(s)) return false;
	PCF8574.wait(us);
	return true;
}

bool jm_LCM2004::command(uint8_t command, uint16_t us)
{
	if (!send4bit((uint8_t) (command & 0xF0), false, 0)) return false;
	if (!send4bit((uint8_t) (command << 4), false, us)) return false;
	return true;
}

int jm_LCM2004::rcve4bit()
{
	uint8_t s[] = {0b11111010,0b11111110,0b11111010}; // set/clr Backlight|RD|RS, pulse E
	if (PCF8574.write((const uint8_t *) &s[0], 2) != 2) return -1;
	int result;
	result = PCF8574.read();
	if (result == -1) return -1;
	if (PCF8574.write((const uint8_t *) &s[2], 1) != 1) return -1;
	return result;
}

int jm_LCM2004::state()
{
	int result1;
	result1 = rcve4bit();
	if (result1 == -1) return -1;
	int result2;
	result2 = rcve4bit();
	if (result2 == -1) return -1;
	return ((result1 & 0xF0) | (result2 >> 4));
}

// ---------------------------------------------------------------------------

bool jm_LCM2004::clear()
{
	return command(HD44780U_CLEAR, 1520); // 1520 ?
}

bool jm_LCM2004::home()
{
	return command(HD44780U_HOME, 1520);
}

bool jm_LCM2004::entry(uint8_t i_d, uint8_t s)
{
	return command(HD44780U_ENTRY | i_d | s, 37);
}

bool jm_LCM2004::display(bool D, bool C, bool B)
{
	return command(HD44780U_DISPLAY | (D ? HD44780U_DISPLAY_D : 0) | (C ? HD44780U_DISPLAY_C : 0) | (B ? HD44780U_DISPLAY_B : 0), 37);
}

bool jm_LCM2004::cursor(bool S_C, bool R_L)
{
	return command(HD44780U_CURSOR | (S_C ? HD44780U_CURSOR_S_C : 0) | (R_L ? HD44780U_CURSOR_R_L : 0), 37);
}

bool jm_LCM2004::set(bool DL, bool N, bool F)
{
	return command(HD44780U_SET | (DL ? HD44780U_SET_DL : 0) | (N ? HD44780U_SET_N : 0) | (F ? HD44780U_SET_F : 0), 37);
}

bool jm_LCM2004::cgram(uint8_t ACG)
{
	return command(HD44780U_CGRAM | (ACG & HD44780U_CGRAM_ACG), 37);
}

bool jm_LCM2004::ddram(uint8_t ADD)
{
	return command(HD44780U_DDRAM | (ADD & HD44780U_DDRAM_ADD), 37);
}

// ---------------------------------------------------------------------------

bool jm_LCM2004::begin()
{
	// Software reset after Power On...

	delay(40);

	if (!send4bit(0b00110000, false, 4100)) return false;
	if (!send4bit(0b00110000, false, 100)) return false;
	if (!send4bit(0b00110000, false, 37)) return false;
	if (!send4bit(0b00100000, false, 37)) return false;

	if (!set()) return false;						// Function set 4 bits, 2 lines, 5x8 dots
	if (!display(false)) return false;				// Display off
	if (!clear()) return false;						// Clear display
	if (!entry()) return false;						// Entry mode set increment
	if (!display(true, true, true)) return false;	// Display on, Cursor on, Blink on

	return true;
}

size_t jm_LCM2004::write(uint8_t value)
{
	if (!send4bit((uint8_t) (value & 0xF0), true, 0)) return 0;
	if (!send4bit((uint8_t) (value << 4), true, 0)) return 0;

	return 1;
}

bool jm_LCM2004::set_cursor(int col, int row)
{
	return ddram(col + ((row & 1) ? 64 : 0) + ((row & 2) ? 20 : 0));
}

// ---------------------------------------------------------------------------

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

	delay(500);

	LCM2004.print("qwertz");

	LCM2004.write('1');
	LCM2004.write('2');
	LCM2004.write('3');
	LCM2004.write('4');

	LCM2004.cursor();

	LCM2004.write("xyz");

	LCM2004.ddram(20);
	LCM2004.write("xyz");

	LCM2004.set_cursor(0,0);
	LCM2004.write('0');
	LCM2004.set_cursor(1,1);
	LCM2004.write('1');
	LCM2004.set_cursor(12,2);
	LCM2004.write('2');
	LCM2004.set_cursor(13,3);
	LCM2004.write('3');

	Serial.println(LCM2004.state(), HEX);
}

void loop()
{
}
