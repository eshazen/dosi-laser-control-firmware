//
// shiftOut(), inspired by Arduino
//

#include <avr/io.h>
#include <stdint.h>
#include "ioport.h"

// this (shiftIn) code is left here from Arduino in case someone
// wants to implement it.

// 
// uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
// 	uint8_t value = 0;
// 	uint8_t i;
// 
// 	for (i = 0; i < 8; ++i) {
// 		digitalWrite(clockPin, HIGH);
// 		if (bitOrder == LSBFIRST)
// 			value |= digitalRead(dataPin) << i;
// 		else
// 			value |= digitalRead(dataPin) << (7 - i);
// 		digitalWrite(clockPin, LOW);
// 	}
// 	return value;
// }
//

// shift 1-8 bits out LSB first
// cycle clock high, low after each bit
void shiftOutLsb( volatile uint8_t *port, uint8_t clockPin, uint8_t dataPin,
	       uint8_t val, uint8_t count) {
  uint8_t i;

  for( i=0; i<count; i++) {
    if( val & 1)
      set_io_bits( port, _BV(dataPin));
    else
      clear_io_bits( port, _BV(dataPin));
    set_io_bits( port, _BV( clockPin));
    clear_io_bits( port, _BV( clockPin));
    val >>= 1;
  }
  clear_io_bits( port, _BV(dataPin));
}


// shift 1-8 bits out MSB first
// cycle clock high, low after each bit
void shiftOutMsb( volatile uint8_t *port, uint8_t clockPin, uint8_t dataPin,
	       uint8_t val, uint8_t count) {
  uint8_t i;

  for( i=0; i<count; i++) {
    if( val & 0x80)
      set_io_bits( port, _BV(dataPin));
    else
      clear_io_bits( port, _BV(dataPin));
    set_io_bits( port, _BV( clockPin));
    clear_io_bits( port, _BV( clockPin));
    val <<= 1;
  }
  clear_io_bits( port, _BV(dataPin));
}


