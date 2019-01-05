
#include <jm_PCF8574.h>

#include <Wire.h>

#include <Arduino.h> 			// HIGH, LOW,...

//#include <jm_Pin.h>				// jm_Pin_ext_pins * jm_PCF8574::port_register();

//------------------------------------------------------------------------------

jm_PCF8574::jm_PCF8574(uint8_t i2c_address) :
	_i2c_address(i2c_address),
	_connected(false),			// device not connected (disconnected)
	_read(0xFF),				// read() INPUT pullup
	_read_pc(0xFF),				// read_pc() INPUT pullup
	_outputs(0x00),				// no declared OUTPUTs
	_shadows(0x00),				// OUTPUT shadows cleared
	_delayed(false)				// Digital Read&Write not delayed (immediate)
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

uint8_t jm_PCF8574::outputs()
{
	return _outputs;
}

void jm_PCF8574::outputs(uint8_t outputs)
{
	_outputs = outputs;
}

uint8_t jm_PCF8574::shadows()
{
	return _shadows;
}

void jm_PCF8574::shadows(uint8_t shadows)
{
	_shadows = shadows;
}

bool jm_PCF8574::delayed()
{
	return _delayed;
}

void jm_PCF8574::delayed(bool delayed)
{
	_delayed = delayed;
}

bool jm_PCF8574::connected()
{
	return _connected;
}

// ---------------------------------------------------------------------------

bool jm_PCF8574::begin()
{
	if (_connected) return true;

	int result;

	Wire.begin();

	Wire.beginTransmission(_i2c_address);
	result = Wire.endTransmission();

	_connected = (result == 0);

	return _connected;
}

void jm_PCF8574::end()
{
	_connected = false;
}

int jm_PCF8574::read()
{
	if (!_connected) return -1;

	int result;

	result = Wire.requestFrom(_i2c_address, (uint8_t)1);
	if (result != 1) {_connected = false; return -1;}

//	result = _read = Wire.read();
	result = Wire.read();
	if (result == -1) {_connected = false; return -1;}

	_read = result;

	return result;
}

size_t jm_PCF8574::write(uint8_t value)
{
	if (!_connected) return 0;

	int result;

	Wire.beginTransmission(_i2c_address);

	result = Wire.write(value);
	if (result != 1) {_connected = false; return 0;}

	result = Wire.endTransmission();
	if (result != 0) {_connected = false; return 0;}

	return 1;
}

size_t jm_PCF8574::write(const uint8_t *data, size_t quantity)
{
	if (!_connected) return 0;

	int result;

	for (size_t i=0; i<quantity; i++) {
		result = write(data[i]);
		if (result != 1) {_connected = false; return i;}
	}

	return quantity;
}

// ---------------------------------------------------------------------------

uint8_t jm_PCF8574::portMode()
{
	return _outputs;
}

void jm_PCF8574::portMode(uint8_t mode)
{
	if (mode == OUTPUT)
		_outputs = 0xFF;	// all OUTPUTs
	else
		_outputs = 0x00;	// no OUTPUTs
}

int jm_PCF8574::portRead()
{
	if (_connected && !_delayed) read();

	return (_read & ~_outputs);
}

void jm_PCF8574::portWrite(uint8_t value)
{
	_shadows = ((_shadows | ~_outputs | (value & _outputs)) & ~(~value & _outputs));

	if (_connected && !_delayed) write(_shadows);
}

// ---------------------------------------------------------------------------

uint8_t jm_PCF8574::pinMask(uint8_t pin)
{
	return (1 << pin);
}

uint8_t jm_PCF8574::pinMode(uint8_t pin)
{
	if (_outputs & (1 << pin))
		return OUTPUT;
	else
		return INPUT; //INPUT_PULLUP;
}

void jm_PCF8574::pinMode(uint8_t pin, uint8_t mode)
{
	if (mode == OUTPUT)
		_outputs |= (1 << pin);
	else
		_outputs &= ~(1 << pin);
}

int jm_PCF8574::digitalRead(uint8_t pin)
{
	if (_connected && !_delayed) read();

	return (_read & (1 << pin) ? HIGH : LOW);
}

void jm_PCF8574::digitalWrite(uint8_t pin, uint8_t value)
{
	uint8_t _value1 = (value ? (1 << pin) : 0);
	uint8_t _output1 = (_outputs & (1 << pin));

	_shadows = ((_shadows | ~_outputs | (_value1 & _output1)) & ~(~_value1 & _output1));

	if (_connected && !_delayed) write(_shadows);
}

// ---------------------------------------------------------------------------

//#include <jm_Pin.h>

void jm_PCF8574::obj_pinMode(void * obj, uint8_t pin, uint8_t mode)
{
	if ((jm_PCF8574 *) obj) ((jm_PCF8574 *) obj)->pinMode(pin, mode);
}

int jm_PCF8574::obj_digitalRead(void * obj, uint8_t pin)
{
	if ((jm_PCF8574 *) obj) return ((jm_PCF8574 *) obj)->digitalRead(pin); else return LOW; //-1;
}

void jm_PCF8574::obj_digitalWrite(void * obj, uint8_t pin, uint8_t value)
{
	if ((jm_PCF8574 *) obj) ((jm_PCF8574 *) obj)->digitalWrite(pin, value);
}

//jm_Pin_ext_pins * jm_PCF8574::port_register()
//{
//	return jm_Pin::port_register(this, jm_PCF8574::obj_pinMode, jm_PCF8574::obj_digitalRead, jm_PCF8574::obj_digitalWrite);
//}

// ---------------------------------------------------------------------------

void jm_PCF8574::wait(uint16_t us)
{
	if (us > 100) delayMicroseconds(us - 100);
}

//------------------------------------------------------------------------------

// END.