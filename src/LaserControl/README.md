# Laser control first version

## Rev 1.0 ECOs

* MOSI and MISO swapped on ISP connector
* R4 and R5 shuld be smaller (red LEDs are quite dim)
* Missing pull-ups on all local I2C buses (sigh!)
* Missing pull-ups on all remote I2C buses (double sigh!)

## Rev 1.0 Debug Log

* Avrdude recognizes the device! <br>
`avrdude -c usbtiny -p usb -p m88`
* Blinky runs with F_CPU @ 1MHz
* UART interface working @ 1200 baud
* I2C not working (always get "I2C ERR") <br>
  _Missing resistors_
* I2C working fine with pull-ups added




