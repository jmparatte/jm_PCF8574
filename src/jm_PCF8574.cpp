
#include <jm_PCF8574.h>

#include <Wire.h>

#include <Arduino.h> 			// HIGH, LOW,...

//------------------------------------------------------------------------------

jm_PCF8574::jm_PCF8574(uint8_t i2c_address) :
	_i2c_address(i2c_address),	// set device I2C address
	_connected(false),			// set device not connected (disconnected)
	_io_mask(0x00),				// set digital I/O mask P0-P7 as INPUT
	_io_data(0xFF)				// set digital I/O data P0-P7 as INPUT
{
}

jm_PCF8574::operator bool()
{
	return _connected;
}

// ---------------------------------------------------------------------------

uint8_t jm_PCF8574::i2c_address()
{
	return _i2c_address;
}

bool jm_PCF8574::connected()
{
	return _connected;
}

// ---------------------------------------------------------------------------

bool jm_PCF8574::begin() // return OK
{
	if (_connected) return true;

	Wire.end();

	Wire.begin();

	_connected = true;

	// check access to device...

	int result;

	Wire.beginTransmission(_i2c_address);
	result = Wire.endTransmission();

	_connected = (result == 0);

	return _connected;
}

void jm_PCF8574::end()
{
	_connected = false;
}

// ---------------------------------------------------------------------------

int jm_PCF8574::read()
{
	if (!_connected) return -1;

	if (Wire.requestFrom(_i2c_address, (uint8_t) 1) != (uint8_t) 1) {_connected = false; return -1;}

	return Wire.read();
}

size_t jm_PCF8574::write(uint8_t value)
{
	if (!_connected) return 0;

	Wire.beginTransmission(_i2c_address);
	if (Wire.write(value) != 1) {_connected = false; return 0;}
	if (Wire.endTransmission(true) != (uint8_t) 0) {_connected = false; return 0;}

	return 1;
}

size_t jm_PCF8574::write(const uint8_t *data, size_t quantity)
{
	if (!_connected) return 0;

	for (size_t i=0; i<quantity; i++) {
		if (write(data[i]) != 1) {_connected = false; return i;}
	}

	return quantity;
}

// ---------------------------------------------------------------------------

uint8_t jm_PCF8574::pinMask(uint8_t pin)
{
	return (1 << pin);
}

uint8_t jm_PCF8574::pinMode(uint8_t pin)
{
	if (_io_mask & (1 << pin))
		return OUTPUT; // OPEN_DRAIN
	else
		return INPUT; // INPUT_PULLUP
}

void jm_PCF8574::pinMode(uint8_t pin, uint8_t mode)
{
	if (mode == OUTPUT)
		_io_mask |= (1 << pin); // OPEN_DRAIN
	else
		_io_mask &= ~(1 << pin); // INPUT_PULLUP

	_io_data |= ~_io_mask; // force digital pin to HIGH if configured as INPUT
}

int jm_PCF8574::digitalRead(uint8_t pin)
{
	int result;

	result = read();
	if (result == -1) return LOW;

	return (result & (1 << pin) ? HIGH : LOW);
}

void jm_PCF8574::digitalWrite(uint8_t pin, uint8_t value)
{
	uint8_t io_data1 = (value ? (1 << pin) : 0);
	uint8_t io_mask1 = (_io_mask & (1 << pin));

	_io_data |= (io_data1 & io_mask1); // set pin to HIGH if value is HIGH and pin is configured as OUTPUT
	_io_data &= ~(~io_data1 & io_mask1); // set pin to LOW if value is LOW and pin is configured as OUTPUT

	write(_io_data);
}

// ---------------------------------------------------------------------------

void jm_PCF8574::wait(uint16_t us)
{
	if (us > 100) delayMicroseconds(us - 100);
}

// ---------------------------------------------------------------------------

// static functions...

void jm_PCF8574::obj_pinMode(void * obj, uint8_t pin, uint8_t mode)
{
	if ((jm_PCF8574 *) obj) ((jm_PCF8574 *) obj)->pinMode(pin, mode);
}

int jm_PCF8574::obj_digitalRead(void * obj, uint8_t pin)
{
	if ((jm_PCF8574 *) obj) return ((jm_PCF8574 *) obj)->digitalRead(pin); else return LOW;
}

void jm_PCF8574::obj_digitalWrite(void * obj, uint8_t pin, uint8_t value)
{
	if ((jm_PCF8574 *) obj) ((jm_PCF8574 *) obj)->digitalWrite(pin, value);
}

//------------------------------------------------------------------------------

// END.