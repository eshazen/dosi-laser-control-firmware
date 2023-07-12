/*
 * UART stdio example
 * note that there is no editing on input!
 */

#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

// Arduino LED is on PB5

#define LED_DDR DDRB
#define LED_BIT 5
#define LED_PORT PORTB

// create a file pointer for read/write to USART0
FILE usart0_str = FDEV_SETUP_STREAM(USART0SendByte, USART0ReceiveByte, _FDEV_SETUP_RW);

static char buff[40];

int main (void)
{
  unsigned char c = ' ';

  USART0Init();
  stdout = &usart0_str;		/* connect UART to stdout */
  stdin = &usart0_str;		/* connect UART to stdin */

  LED_DDR ^= (1 << LED_BIT);

  puts("UART test");

  while( 1) {
    puts("> ");
    fgets( buff, sizeof( buff), stdin);
    for( int i=0; i<strlen(buff); i++) {
      printf("%d: ", i);
      unsigned char c = buff[i];
      if( c < 0x20 || c > 0x7f)
	printf("0x%x\n", c);
      else
	printf("%c\n", c);
    }
  }
}


