
#define __PROG__ "jm_PCF8574_demo4"

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

#define LCM2004_I2C_DB		((uint8_t) 0b11110000)
#define LCM2004_I2C_DB7		((uint8_t) 0b10000000)
#define LCM2004_I2C_DB6		((uint8_t) 0b01000000)
#define LCM2004_I2C_DB5		((uint8_t) 0b00100000)
#define LCM2004_I2C_DB4		((uint8_t) 0b00010000)
#define LCM2004_I2C_LED		((uint8_t) 0b00001000)
#define LCM2004_I2C_E		((uint8_t) 0b00000100)
#define LCM2004_I2C_R_W		((uint8_t) 0b00000010)
#define LCM2004_I2C_RS		((uint8_t) 0b00000001)

class jm_LCM2004_I2C : public Print
{
private:

protected:
	bool _LED;

	bool wr_4bit(uint8_t DB, bool RS, uint16_t us);
	int rd_4bit(bool RS);

public:
	bool wr_command(uint8_t command, uint16_t us);
	int rd_command();

	jm_LCM2004_I2C();

	operator bool();

	bool clear_display();
	bool return_home();
	bool entry_mode_set(bool I_D=true, bool S=false);
	bool display_control(bool D=true, bool C=false, bool B=false);
	bool cursor_shift(bool S_C=false, bool R_L=true);
	bool function_set(bool DL=false, bool N=true, bool F=false); // 4 bits, 2 lines, 5x8 dots
	bool set_cgram_addr(uint8_t ACG=0);
	bool set_ddram_addr(uint8_t ADD=0);

	bool begin();

	virtual size_t write(uint8_t value);
	inline size_t write(unsigned long n) {return write((uint8_t) n);}
	inline size_t write(long n) {return write((uint8_t) n);}
	inline size_t write(unsigned int n) {return write((uint8_t) n);}
	inline size_t write(int n) {return write((uint8_t) n);}
	using Print::write;

	bool set_cursor(int col, int row);

	bool character_generator(uint8_t location, uint8_t charmap[]);
};

// ---------------------------------------------------------------------------

bool jm_LCM2004_I2C::wr_4bit(uint8_t DB, bool RS, uint16_t us)
{
	uint8_t s[3];
	for (size_t i=0; i<3; i++)
		s[i] = (
			(DB & LCM2004_I2C_DB) |
			(_LED ? LCM2004_I2C_LED : 0) |
			((i & 1) ? LCM2004_I2C_E : 0) |
			0 |
			(RS ? LCM2004_I2C_RS : 0)
		);
	if (PCF8574.write((const uint8_t *) &s, 3) != 3) return false;
	PCF8574.wait(us + us/10); // add 10% HD44780U frequency shifting/error (p.25)

	return true;
}

int jm_LCM2004_I2C::rd_4bit(bool RS)
{
	uint8_t s[3];
	for (size_t i=0; i<3; i++)
		s[i] = (
			LCM2004_I2C_DB |
			(_LED ? LCM2004_I2C_LED : 0) |
			((i & 1) ? LCM2004_I2C_E : 0) |
			LCM2004_I2C_R_W |
			(RS ? LCM2004_I2C_RS : 0)
		);
	if (PCF8574.write((const uint8_t *) &s[0], 2) != 2) return -1;
	int result; if ((result = PCF8574.read()) == -1) return -1;
	if (PCF8574.write((const uint8_t *) &s[2], 1) != 1) return -1;

	return result;
}

// ---------------------------------------------------------------------------

bool jm_LCM2004_I2C::wr_command(uint8_t command, uint16_t us)
{
	if (!wr_4bit((uint8_t) (command & 0xF0), false, 0)) return false;
	if (!wr_4bit((uint8_t) (command << 4), false, us)) return false;

	return true;
}

int jm_LCM2004_I2C::rd_command()
{
	int result1; if ((result1 = rd_4bit(false)) == -1) return -1;
	int result2; if ((result2 = rd_4bit(false)) == -1) return -1;

	return ((result1 & 0xF0) | (result2 >> 4));
}

// ---------------------------------------------------------------------------

jm_LCM2004_I2C::jm_LCM2004_I2C() :
	_LED(true)
{
}

jm_LCM2004_I2C::operator bool()
{
	return true;
}

// ---------------------------------------------------------------------------

bool jm_LCM2004_I2C::clear_display()
{
	return wr_command(HD44780U_CLEAR, 1520); // 1520 ?
}

bool jm_LCM2004_I2C::return_home()
{
	return wr_command(HD44780U_HOME, 1520);
}

bool jm_LCM2004_I2C::entry_mode_set(bool I_D, bool S)
{
	return wr_command(HD44780U_ENTRY | (I_D ? HD44780U_ENTRY_I_D : 0) | (S ? HD44780U_ENTRY_S : 0), 37);
}

bool jm_LCM2004_I2C::display_control(bool D, bool C, bool B)
{
	return wr_command(HD44780U_DISPLAY | (D ? HD44780U_DISPLAY_D : 0) | (C ? HD44780U_DISPLAY_C : 0) | (B ? HD44780U_DISPLAY_B : 0), 37);
}

bool jm_LCM2004_I2C::cursor_shift(bool S_C, bool R_L)
{
	return wr_command(HD44780U_CURSOR | (S_C ? HD44780U_CURSOR_S_C : 0) | (R_L ? HD44780U_CURSOR_R_L : 0), 37);
}

bool jm_LCM2004_I2C::function_set(bool DL, bool N, bool F)
{
	return wr_command(HD44780U_SET | (DL ? HD44780U_SET_DL : 0) | (N ? HD44780U_SET_N : 0) | (F ? HD44780U_SET_F : 0), 37);
}

bool jm_LCM2004_I2C::set_cgram_addr(uint8_t ACG)
{
	return wr_command(HD44780U_CGRAM | (ACG & HD44780U_CGRAM_ACG), 37);
}

bool jm_LCM2004_I2C::set_ddram_addr(uint8_t ADD)
{
	return wr_command(HD44780U_DDRAM | (ADD & HD44780U_DDRAM_ADD), 37);
}

// ---------------------------------------------------------------------------

bool jm_LCM2004_I2C::begin()
{
	// Software reset after Power On...

	delay(40);

	if (!wr_4bit(HD44780U_SET | HD44780U_SET_DL, false, 4100)) return false;
	if (!wr_4bit(HD44780U_SET | HD44780U_SET_DL, false, 100)) return false;
	if (!wr_4bit(HD44780U_SET | HD44780U_SET_DL, false, 37)) return false;
	if (!wr_4bit(HD44780U_SET                  , false, 37)) return false;

	if (!function_set()) return false;						// Function set 4 bits, 2 lines, 5x8 dots
	if (!display_control(false)) return false;				// Display off
	if (!clear_display()) return false;						// Clear display
	if (!entry_mode_set()) return false;					// Entry mode set increment
//	if (!display_control(true, true, true)) return false;	// Display on, Cursor on, Blink on
	if (!display_control(true)) return false;				// Display on, Cursor off, Blink off

	return true;
}

size_t jm_LCM2004_I2C::write(uint8_t value)
{
	if (!wr_4bit((uint8_t) (value & 0xF0), true, 0)) return 0;
	if (!wr_4bit((uint8_t) (value << 4), true, 0)) return 0;

	return 1;
}

bool jm_LCM2004_I2C::set_cursor(int col, int row)
{
	return set_ddram_addr(col + ((row & 1) ? 64 : 0) + ((row & 2) ? 20 : 0));
}

bool jm_LCM2004_I2C::character_generator(uint8_t location, uint8_t charmap[])
{
	if (!set_cgram_addr((location & 7) * 8)) return false;

	for (size_t i=0; i<8; i++)
	{
		write(charmap[i]);
	}

	return true;
}

// ---------------------------------------------------------------------------

jm_LCM2004_I2C LCM2004_I2C;

////////////////////////////////////////////////////////////////////////////////

const uint8_t charBitmap[8][8] = {
   { 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F }
};

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

	LCM2004_I2C.begin();

	delay(500);

	LCM2004_I2C.print("qwertz");

	LCM2004_I2C.write('1');
	LCM2004_I2C.write('2');
	LCM2004_I2C.write('3');
	LCM2004_I2C.write('4');

	LCM2004_I2C.cursor_shift();

	LCM2004_I2C.write("xyz");

	LCM2004_I2C.set_ddram_addr(20);
	LCM2004_I2C.write("xyz");

	LCM2004_I2C.set_cursor(0,0);
	LCM2004_I2C.write('0');
	LCM2004_I2C.set_cursor(1,1);
	LCM2004_I2C.write('1');
	LCM2004_I2C.set_cursor(12,2);
	LCM2004_I2C.write('2');
	LCM2004_I2C.set_cursor(13,3);
	LCM2004_I2C.write('3');

	Serial.println(LCM2004_I2C.rd_command(), HEX);

//	LCM2004_I2C.display_control();

	LCM2004_I2C.clear_display();

	LCM2004_I2C.set_cursor(0,0);
	LCM2004_I2C.write('0');
	LCM2004_I2C.set_cursor(1,1);
	LCM2004_I2C.write('1');
	LCM2004_I2C.set_cursor(12,2);
	LCM2004_I2C.write('2');
	LCM2004_I2C.set_cursor(13,3);
	LCM2004_I2C.write('3');

	Serial.println(LCM2004_I2C.rd_command(), HEX);

	for (int i=0; i<8; i++)
	{
		LCM2004_I2C.character_generator(i, (uint8_t *) charBitmap[i]);
	}

	LCM2004_I2C.set_cursor(10,0);
	for (uint8_t c=0; c<8; c++) LCM2004_I2C.write(c);
}

void loop()
{
}
