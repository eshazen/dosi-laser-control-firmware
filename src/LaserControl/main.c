/*
 * DOSI Laser control software
 * 
 */

#define DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <ctype.h>
#include <avr/pgmspace.h>
#include "uart.h"
#include "../libs/ioport.h"

#include "I2C_BB.h"
#include "laser_io.h"
#include "parse.h"

// Arduino LED is on PB5

#define LED_DDR DDRB
#define LED_BIT 5
#define LED_PORT PORTB

// create a file pointer for read/write to USART0
// not in C++!
static char buff[40];
static char tmp[10];

void setup() {
  set_io_bits( &LASER_ENA_DDR, _BV(LASER_ENA_PIN));
  clear_io_bits( &LASER_ENA_PORT, _BV(LASER_ENA_PIN));
  i2c_begin();
}

FILE usart0_str = FDEV_SETUP_STREAM(USART0SendByte, USART0ReceiveByte, _FDEV_SETUP_RW);

#define MAXTOK 4
int iargv[MAXTOK];
char *argv[MAXTOK];

// strings in flash
const char menu[] PROGMEM =
  "  H       - help\n"  \
  "  E l     - disable\n" \
  "  D [l]   - enable\n" \
  "  L v     - set LEDs\n" \
  "  I l v   - set laser l current to v\n" \
  "  R l [c] - read laser channel\n";
const char hello[] PROGMEM = "DOSI Laser Control v1.0";

int main (void)
{
  setup();

  unsigned char c = ' ';

  USART0Init();
  stdout = &usart0_str;		/* connect UART to stdout */
  stdin = &usart0_str;		/* connect UART to stdin */

  LED_DDR ^= (1 << LED_BIT);

  puts_P( hello);
  
  while( 1) {
    putchar('>');

    USART0GetString( buff, sizeof( buff));

    int argc = parse( buff, argv, iargv, MAXTOK);

    switch( toupper( *buff)) {
    case 'H':
      puts_P( menu);
      break;

    case 'E':
      if( argc >= 2) {
	printf("EN %d\n", iargv[1]);
      }	
      break;

    case 'D':
      if( argc >= 2) {
	printf("DI %d\n", iargv[1]);
      } else {
	puts("DI ALL");
      }
      break;

    default:
      puts("ERR");
      break;
      
    }
  }
}


