#include <stdint.h>

void shiftOutLsb( volatile uint8_t *port, uint8_t clockPin, uint8_t dataPin, uint8_t val, uint8_t count);
void shiftOutMsb( volatile uint8_t *port, uint8_t clockPin, uint8_t dataPin, uint8_t val, uint8_t count);


