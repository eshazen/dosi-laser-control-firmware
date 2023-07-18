#include <avr/io.h>

#define USART_BAUDRATE 1200
#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define kbhit USART0CharacterAvailable

void USART0Init(void);
int USART0SendByte(char u8Data, FILE *stream);
int USART0CharacterAvailable();
int USART0ReceiveByte( FILE *stream);
void USART0GetString( char *buffer, int max);
