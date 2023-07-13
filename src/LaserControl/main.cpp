/*
 * UART stdio example
 * note that there is no editing on input!
 */

#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <ctype.h>
#include "uart.hh"
#include "../libs/ioport.h"

#include "laser_io.h"


// Arduino LED is on PB5

#define LED_DDR DDRB
#define LED_BIT 5
#define LED_PORT PORTB

// create a file pointer for read/write to USART0
// not in C++!
static char buff[40];

void setup() {
  set_io_bits( &LASER_ENA_DDR, _BV(LASER_ENA_PIN));
  clear_io_bits( &LASER_ENA_PORT, _BV(LASER_ENA_PIN));
}

FILE usart0_str;

int main (void)
{
  // FILE usart0_str = FDEV_SETUP_STREAM(USART0SendByte, USART0ReceiveByte, _FDEV_SETUP_RW);
  usart0_str.put = USART0SendByte;
  usart0_str.get = USART0ReceiveByte;
  usart0_str.flags = _FDEV_SETUP_RW;

  setup();

  unsigned char c = ' ';

  USART0Init();
  stdout = &usart0_str;		/* connect UART to stdout */
  stdin = &usart0_str;		/* connect UART to stdin */

  LED_DDR ^= (1 << LED_BIT);

  puts("UART test");

  while( 1) {
    puts("> ");
    fgets( buff, sizeof( buff), stdin);
    switch( toupper( *buff)) {
    case 'H':
      puts("H     - help");
      puts("E0    - disable");
      puts("E1    - enable");
      break;

    case 'E':
      if( buff[1] == '0')
	clear_io_bits( &LASER_ENA_PORT, _BV( LASER_ENA_PIN));
      else
	set_io_bits( &LASER_ENA_PORT, _BV( LASER_ENA_PIN));
      break;

    default:
      puts("ERR");
      break;
      
    }
  }
}


