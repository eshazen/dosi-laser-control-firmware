//
// set bit(s) in I/O port
//
// call as set_io_bits( &PORTB, _BV(3) );
// this seems easy but the "volatile" is necessary else
// it doesn't work, and also the &PORT syntax
//

#include "ioport.h"

void clear_io_bits( volatile uint8_t *port, uint8_t mask)
{
  *port &= ~mask;
}

void set_io_bits( volatile uint8_t *port, uint8_t mask)
{
  *port |= mask;
}

uint8_t get_io_bits( volatile uint8_t *port, uint8_t mask)
{
  return *port & mask;
}
