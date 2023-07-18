/*
 * blinky example
 */

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

// Arduino LED is on PB5

#define LED_DDR DDRD
#define LED1_BIT PD3
#define LED2_BIT PD6
#define LED_PORT PORTD

int main (void)
{

  LED_DDR |= _BV(LED1_BIT) | _BV(LED2_BIT);

  while( 1) {
    LED_PORT |= _BV(LED1_BIT);
    _delay_ms( 500);
    LED_PORT &= ~_BV(LED1_BIT);
    _delay_ms( 500);
    LED_PORT |= _BV(LED2_BIT);
    _delay_ms( 500);
    LED_PORT &= ~_BV(LED2_BIT);
    _delay_ms( 500);
  }
}


