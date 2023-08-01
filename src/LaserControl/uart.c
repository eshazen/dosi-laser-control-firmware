//
// very simple UART functions
//

#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "uart.h"

// initialize the UART.  Baud rate set in uart.h
void USART0Init(void)
{
  // Set baud rate
  UBRR0H = (uint8_t)(UBRR_VALUE>>8);
  UBRR0L = (uint8_t)UBRR_VALUE;
  // Set frame format to 8 data bits, no parity, 1 stop bit
  UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
  //enable transmission and reception
  UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
}

// send a byte.  stream is provided for stdio compatibility
// and may be NULL
// convert <LF> to <CR>,<LF>
int USART0SendByte(char u8Data, FILE *stream)
{
  if(u8Data == '\n')
    {
      USART0SendByte('\r', stream);
    }
  //wait while previous byte is completed
  while(!(UCSR0A&(1<<UDRE0))){};
  // Transmit data
  UDR0 = u8Data;
  return 0;
}


// return true if a character is available for input
int USART0CharacterAvailable()
{
  return (UCSR0A&(1<<RXC0));
}

// receive a byte, block in none available
// argument is ignored
// DON'T echo character
// convert <CR> to <LF>
int USART0ReceiveByte( FILE *stream)
{
  unsigned char c;
  // Wait for byte to be received
  while(!(UCSR0A&(1<<RXC0))){};
  // Return received data
  c = UDR0;
  if(c == '\r') {
    c = '\n';
  }
  return c;
}

//
// read string with minimal editing
//   (only backspace recognized)
// ^P will echo existing string for recall
//
void USART0GetString( char *buffer, int max)
{
  char *p = buffer;
  uint8_t c;
  uint8_t n = strlen(buffer);
  bool recall = false;

  while( 1) {
    c = USART0ReceiveByte( NULL);
    if( c == '\x10') {
      // recall the buffer... so echo it
      if( n < max && !recall) {
	fputs( buffer, stdout);
	// point p at end
	p = buffer+n;
	recall = true;
      }
    }
    if( c == '\n') {
      putchar( c);
      *p++ = '\0';
      return;
    }
    if( (c == '\b' || c == '\x7f') && p > buffer) {
      putchar( '\b');
      --p;
    } else {
      if( (p - buffer < max) && isprint(c) ) {
	putchar( c);
	*p++ = c;
      }
    }
    
  }
  
}
