# Laser control first version

Hardware ECOS:  https://gitlab.com/eshazen/i2c-isolator-test/-/tree/main/iso-master-fmc

## Revs for Laser board V2.1

* Change order of display so PMON and IMON are correct
* Change fuse to remove CLKDIV8 (now serial port is 9600 baud)
* Add "G" command for quick data collection

## To Do

* Add timeouts for SCL/SDA stuck low
* [not needed?] Reset I2C bus Mux each time used
* [DONE] add "masked" laser enable (e.g. "E 001100")

## Rev 1.0 Debug Log

* Avrdude recognizes the device! <br>
`avrdude -c usbtiny -p usb -p m88`
* Blinky runs with F_CPU @ 1MHz
* UART interface working @ 1200 baud
* I2C not working (always get "I2C ERR") <br>
  _Missing resistors_
* I2C working fine with pull-ups added

