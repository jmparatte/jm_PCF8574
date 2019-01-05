
#ifndef jm_PCF8574_h
#define jm_PCF8574_h

#include <stddef.h>
#include <inttypes.h>

//#include <jm_Pin.h> // jm_Pin_ext_pins * port_register();

#ifndef NO_CHANGE
#define NO_CHANGE (0)
#endif

class jm_PCF8574
{
private:

protected:
	uint8_t	_i2c_address;		// I2C address
	bool	_connected;			// Device connected
	uint8_t	_read;				// Last I2C read value
	uint8_t	_read_pc;			// Last I2C read value PinChange
	uint8_t _outputs;			// Outputs mask
	uint8_t	_shadows;			// Shadows data
	bool	_delayed;			// Digital Read&Write delayed

public:
	jm_PCF8574(uint8_t i2c_address);

	operator bool();

	uint8_t i2c_address();

	bool connected();

	uint8_t outputs();
	void outputs(uint8_t outputs);

	uint8_t shadows();
	void shadows(uint8_t shadows);

	bool delayed();
	void delayed(bool delayed);

	bool begin();
	void end();

	int read();
	size_t write(uint8_t value);
	size_t write(const uint8_t *data, size_t quantity);

	uint8_t portMode();
	void portMode(uint8_t mode);
	int portRead();
	void portWrite(uint8_t value);

	uint8_t pinMask(uint8_t pin);
	uint8_t pinMode(uint8_t pin);
	void pinMode(uint8_t pin, uint8_t mode);
	int digitalRead(uint8_t pin);
	void digitalWrite(uint8_t pin, uint8_t value);

	static void obj_pinMode(void * obj, uint8_t pin, uint8_t mode);
	static int obj_digitalRead(void * obj, uint8_t pin);
	static void obj_digitalWrite(void * obj, uint8_t pin, uint8_t value);

//	jm_Pin_ext_pins * port_register();

	void wait(uint16_t us);
};

#endif // jm_PCF8574_h
