# jm_PCF8574 Arduino Library

2020-05-11: v1.0.7 - Checked with architectures AVR, SAM and ESP32.  
2020-05-02: v1.0.6 - `void end()` changed to `bool end()`. `void wait()` changed to `bool wait()`.  
2020-05-02: v1.0.5 - README.md updated. `write(data, quantity)` updated. New folder `extras`.  
2019-01-08: v1.0.1 - Checked for ESP32.  
2019-01-05: v1.0.0 - Initial commit.



### PCF8574/PCF8574A Device - Remote 8-bit I/O expander for I2C-bus with interrupt

##### Device datasheet

- http://www.ti.com/lit/ml/scyb031/scyb031.pdf
- http://www.ti.com/lit/ds/symlink/pcf8574.pdf
- https://www.nxp.com/docs/en/data-sheet/PCF8574_PCF8574A.pdf

##### Example application usage

- The LCM2004A LCD module (HD44780U device) interfaced for I2C with a PCF8574.

##### Quasi-bidirectional I/Os P0..P7

The PCF8574 design is very simple: each pin of the eight P0..P7 could be an INPUT_PULLUP or a OPEN_DRAIN. A logical "1" sets the pin to high level through a low current pullup resistor, it's a INPUT_PULLUP input mode. A logical "0" sets the pin to low level through a low current open drain transistor, it's a OPEN_DRAIN output mode. To speed transition toward low level, the current is overloaded during the transition.

##### Device reset

After device reset, all pins are set to logical "1" and operate as INPUT_PULLUP. There is no I2C command to reset the device else power down it.

##### Device connected/disconnected

`begin()`connects the device and `end()` or any I2C errors disconnect it. If device is disconnected, `read()` or `write()` fail immediately, the device needs to be reconnected. The `connected`status is readable with `bool()`operator or `connected()`property.



### jm_PCF8574 Class

```
class jm_PCF8574
{
public:

jm_PCF8574();
jm_PCF8574(uint8_t i2c_address);
operator bool();

uint8_t i2c_address();
bool connected();

bool begin();
bool begin(uint8_t i2c_address);
void end();

int read();
size_t write(uint8_t value);
size_t write(const uint8_t *data, size_t quantity);

void pinMode(uint8_t pin, uint8_t mode);
int digitalRead(uint8_t pin);
void digitalWrite(uint8_t pin, uint8_t value);
};
```

##### Constructors and properties

```
jm_PCF8574();
jm_PCF8574(uint8_t i2c_address);
operator bool();

uint8_t i2c_address();
bool connected(); 
```

The constructor can optionally set the `i2c_address`, or the `i2c_address` can be fixed later with `begin()`.
The `bool` operator returns the `connected` status of the device, `true` if `connected` and not `I2C errored`, `false` else.

##### Begin and end methods

```
bool begin();
bool begin(uint8_t i2c_address);
void end();
```

- `begin()` connects the device and checks the connectivity with it, returns `true` if `connected`.
- `end()`disconnects the device.

##### Read and write methods

```
int read();
size_t write(uint8_t value);
size_t write(const uint8_t *data, size_t quantity);
```

- `read()` reads the eight Quasi-bidirectional I/Os P0..P7.
- `write(value)`writes `value` to the eight Quasi-bidirectional I/Os P0..P7.
- `write(data, quantity)`writes `data` to the Quasi-bidirectional I/Os P0..P7. Each `data` is written at `I2C speed` (default speed 100 kbits/s).

##### PinMode, digitalRead and digitalWrite methods

```
void pinMode(uint8_t pin, uint8_t mode);
int digitalRead(uint8_t pin);
void digitalWrite(uint8_t pin, uint8_t value);
```

These methods reproduce the behaviors of Arduino digital pins. Pins are numbered 0..7.

- `pinMode(pin, mode)` sets one bit of the Quasi-bidirectional I/Os P0..P7 as INPUT or OUTPUT. INPUT could be more understood as an INPUT_PULLUP and OUTPUT as a OPEN_DRAIN due to the PCF8574 design.
- `digitalRead(pin)` reads one of the eight Quasi-bidirectional I/Os P0..P7.
- `digitalWrite(pin, value)`writes one of the eight Quasi-bidirectional I/Os P0..P7.



### jm_PCF8574_blink.ino

- Blinking backlight of a LCD module.

This example blinks the backlight of a LCM2004A I2C LCD module.  
The LCM2004A module is commonly interfaced for I2C with a PCF8574A or optionaly a PCF8574.  
The default I2C address of the module is commonly 0x3F or optionaly 0x27.  
The LCD display is driven by a Hitachi HD447800.  
Common color of display is blue or optionaly yellow.  
Common size is 20 columns by 4 rows.

##### LCM2004A models

- https://www.banggood.com/search/i2c-2004-204-20-x-4-character-lcd-display-module.html?from=nav
- https://ch.farnell.com/powertip/pc2004ars-awa-a-q/lcd-modul-stn-reflect-20x4/dp/1671507

##### LCM2004A datasheets

- https://www.beta-estore.com/download/rk/RK-10290_410.pdf
- http://www.farnell.com/datasheets/50586.pdf

##### LCM2004A schematic

- https://www.mpja.com/download/35054opdata.pdf

##### HD447800 datasheet

- https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
