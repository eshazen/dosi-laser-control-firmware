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
#include <stdbool.h>
#include <ctype.h>
#include <avr/pgmspace.h>
#include "uart.h"
#include "../libs/ioport.h"

#include "i2c_wiki.h"
#include "laser_io.h"
#include "parse.h"

// create a file pointer for read/write to USART0
// not in C++!
static char buff[40];
static char tmp[10];

static uint16_t adc[8];

static int i;
static int n;

void setup() {
  i2c_begin();
  laser_setup();
}

FILE usart0_str = FDEV_SETUP_STREAM(USART0SendByte, USART0ReceiveByte, _FDEV_SETUP_RW);

#define MAXTOK 4
int iargv[MAXTOK];
char *argv[MAXTOK];

// strings in flash
const char menu[] PROGMEM =
  "  H       - help\n"  \
  "  X       - debug read\n" \
  "  E l     - enaable\n" \
  "  D [l]   - disable\n" \
  "  L v     - set LEDs\n" \
  "  I l v   - set laser l current to v\n" \
  "  N l v   - set laser current (non-volatile)\n" \
  "  C       - clear I2C errors\n" \
  "  S l     - select I2C bus l\n" \
  "  R l [c] - read laser channel\n";
const char errmsg[] PROGMEM = "CMD ERR";
const char erri2c_s[] PROGMEM = "I2C SEL ERR";
const char erri2c_w[] PROGMEM = "I2C WRT ERR";
const char erri2c_r[] PROGMEM = "I2C RD ERR";

// strings in RAM for now
char *adc_name[] = { "IMON", "PMON", "VSET", "TEMP", " VCC" };

int main (void)
{
  setup();

  unsigned char c = ' ';

  USART0Init();
  stdout = &usart0_str;		/* connect UART to stdout */
  stdin = &usart0_str;		/* connect UART to stdin */

  puts_P( PSTR("DOSI Laser Control v0.9"));
  
  while( 1) {

    if( i2c_error())
      puts_P( PSTR("I2C ERROR"));

    putchar('>');

    USART0GetString( buff, sizeof( buff));

    int argc = parse( buff, argv, iargv, MAXTOK);

    switch( toupper( *buff)) {
    case 'H':
      puts_P( menu);
      break;

    case 'S':
      if( argc >= 1) {
	if( laser_sel_chan( iargv[1])) {
	  puts_P( erri2c_s);
	}
      } else
	puts_P( errmsg);
      break;

    case 'C':
      I2C_RST_PORT &= ~_BV(I2C_RST_PIN);
      _delay_us(10);
      I2C_RST_PORT |= _BV(I2C_RST_PIN);
      break;

    case 'L':
      if( argc >= 2)
	laser_leds( iargv[1]);
      else
	puts_P( errmsg);
      break;

    case 'E':
      if( argc >= 2)
	laser_enable( iargv[1], true);
      else
	puts_P( errmsg);
      break;

    case 'D':
      if( argc >= 2) {
	laser_enable( iargv[1], false);
      } else {
	for( uint8_t i=0; i<6; i++)
	  laser_enable( i, false);
      }
      break;

    case 'N':
      if( argc < 3)
	puts_P( errmsg);
      else {
	if( laser_sel_chan( iargv[1]))
	  puts_P( erri2c_s);
	if(laser_set_pot( iargv[2], 2))
	  puts_P( erri2c_w);
      }
      break;

    case 'I':
      if( argc < 3)
	puts_P( errmsg);
      else {
	if( laser_sel_chan( iargv[1]))
	  puts_P( erri2c_s);
	if(laser_set_pot( iargv[2], 0))
	  puts_P( erri2c_w);
      }
      break;

    case 'X':			/* debug */
      do {
#ifdef DEBUG
	i2c_start_cond();
	_delay_us(50);
	i2c_stop_cond();
	_delay_us(50);
#else	
	laser_read_adc( adc);
#endif
	puts("*");
	_delay_ms( 100);
      } while( !kbhit());
      break;
      

    case 'R':
      if( argc < 2)
	puts_P( errmsg);
      else {
	if( laser_sel_chan( iargv[1]))
	  puts_P( erri2c_s);
	if( laser_read_adc( adc))
	  puts_P( erri2c_r);
	else {
	  for( i=0; i<5; i++) {
	    fputs( adc_name[i], stdout);
	    putchar( ' ');
	    format_mv( tmp, adc[i]);
	    fputs( tmp, stdout);
	    putchar( '\n');
	  }
	}
      }
      break;

    default:
	  puts_P( errmsg);
      break;
      
    }
  }
}


