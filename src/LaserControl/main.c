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
#include "format_mv.h"

// create a file pointer for read/write to USART0
// not in C++!
static char buff[40];		/* input buffer */
static char save[40];		/* copy for recall */

char tmp[10];

// 8 words for ADC, 16 words for d-pot
static uint16_t adc[16];

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
  "  E l     - enable\n" \
  "  D [l]   - disable\n" \
  "  L v     - set LEDs\n" \
  "  I l mv  - set laser VSET in mv\n" \
  "  N l mv  - set laser VSET (non-volatile)\n" \
  "  C       - clear I2C errors\n" \
  "  S l     - select I2C bus l\n" \
  "  P l     - dump digital pot\n" \
  "  P l r v - set digital pot reg\n" \
  "  R l     - read laser ADC\n";
const char errmsg[] PROGMEM = "CMD ERR";
const char erri2c_s[] PROGMEM = "I2C SEL ERR";
const char erri2c_w[] PROGMEM = "I2C WRT ERR";
const char erri2c_r[] PROGMEM = "I2C RD ERR";

// strings in RAM for now
// NOTE:  swap the first 2 and update ADC_CH_IMON when PCB design fixed
char *adc_name[] = { "*PMON", "*IMON", " VSET", " TEMP", "  VCC" };
// IMON channel for special treatment
#define ADC_CH_IMON 1

int main (void)
{
  setup();

  unsigned char c = ' ';

  USART0Init();
  stdout = &usart0_str;		/* connect UART to stdout */
  stdin = &usart0_str;		/* connect UART to stdin */

  puts_P( PSTR("DOSI Laser Control v1.1"));
  
  while( 1) {

    if( i2c_error())
      puts_P( PSTR("I2C ERROR"));

    putchar('>');

    USART0GetString( buff, sizeof( buff));
    memcpy( save, buff, sizeof(buff));

    int argc = parse( save, argv, iargv, MAXTOK);
    char cmd = toupper( *argv[0]);

    switch( cmd) {
    case 'H':
      puts_P( menu);
      break;

    // write or read POT registers
    //   "P n"      - dump 16 registers from pot attached to channel n
    //   "P n r v"  - write 1 register r with value v
    //
    case 'P':
      if( argc == 2) {
	if( laser_sel_chan( iargv[1]))
	  puts_P( erri2c_s);
	if( laser_read_pot( adc))
	  puts_P( erri2c_r);
	for( i=0; i<16; i++) {
	  itoa( i, tmp, 10);
	  fputs( tmp, stdout);
	  fputs( " 0x", stdout);
	  itoa( adc[i], tmp, 16);
	  puts( tmp);
	}
      } else if( argc == 4) {
	if( laser_sel_chan( iargv[1]))
	  puts_P( erri2c_s);
	if( laser_set_pot( iargv[3], iargv[2]))
	  puts_P( erri2c_w);
      } else
	puts_P( errmsg);
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
      if( argc >= 2) {
	char *s = argv[1];
	if( strlen( s) == 6) { /* binary enable string */
	  for( i=0; i<6; i++)
	    laser_enable( i, s[i] == '1');
	} else {
	  laser_enable( iargv[1], true);
	}
      } else
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

    // handle both non-volatile and volatile digital pot settings
    case 'N':
    case 'I':
      if( argc < 3)
	puts_P( errmsg);
      else {
	if( laser_sel_chan( iargv[1]))
	  puts_P( erri2c_s);
	// interpret input as mV (0-2500)
	n = 256 - (iargv[2] / 10);
	if( n < 0 || n > 256)
	  puts_P( errmsg);
	else {
	  if( laser_set_pot( n, cmd == 'N' ? 2 : 0))
	    puts_P( erri2c_w);
	}
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
	    n = adc[i];
	    if( i == 3)
	      format_degc( tmp, n);
	    else if( i == ADC_CH_IMON) {
	      // divide V(IMON) by 9.4 (or 5/47) to get laser current directly
	      n = n * 5 / 47;
	      format_mv( tmp, n, 'A');
	    } else {
	      format_mv( tmp, n, 'V');
	    }
	    fputs( tmp, stdout);
	    putchar( '\n');
	  }
	  puts("* Swap on V2");
	}
      }
      break;

    default:
	  puts_P( errmsg);
      break;
      
    }
  }
}


