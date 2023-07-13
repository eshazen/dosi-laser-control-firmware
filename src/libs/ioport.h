#ifndef IOPORT_H
#define IOPORT_H

#include <stdint.h>


void clear_io_bits( volatile uint8_t *port, uint8_t mask);
void set_io_bits( volatile uint8_t *port, uint8_t mask);
uint8_t get_io_bits( volatile uint8_t *port, uint8_t mask);


#endif
