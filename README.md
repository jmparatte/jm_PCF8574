# jm_PCF8574 Arduino Library

````
2019-01-08: v1.0.1 - Checked for ESP32
2019-01-05: v1.0.0 - Initial commit
````

### PCF8574/PCF8574A - Remote 8-bit I/O expander for I2C-bus with interrupt

##### Device datasheet

- http://www.ti.com/lit/ml/scyb031/scyb031.pdf
- http://www.ti.com/lit/ds/symlink/pcf8574.pdf
- https://www.nxp.com/docs/en/data-sheet/PCF8574_PCF8574A.pdf

##### Application example

- The LCM2004A LCD module (HD44780U device) interfaced for I2C with a PCF8574.

### Example

##### jm_PCF8574_blink

	This example blinks the backlight of a LCM2004A I2C LCD module.
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