#ifndef LASER_IO_H
#define LASER_IO_H

#include <stdint.h>
#include <avr/io.h>

// pinout for UNO for testing
#ifdef UNO_PINS

#define LASER_ENA_PORT PORTD
#define LASER_ENA_DDR DDRD

#define LASER_ENA_PIN PD7

#define TWI_PORT PORTC
#define TWI_DDR  DDRC

#define TWI_SDA_BIT PC4
#define TWI_SCL_BIT PC5

#endif


// pinout for FMC
#ifdef FMC_PINS

// enables
static struct {
  volatile uint8_t *port;
  volatile uint8_t *ddr;
  uint8_t pin;
} enables[] = {
  &PORTB, &DDRB, PB2,		/* ch 1 - EN1 */
  &PORTB, &DDRB, PB0,		/* ch 2 - EN0 */
  &PORTB, &DDRB, PB6,		/* ch 3 - EN3 */
  &PORTC, &DDRC, PC2,		/* ch 4 - EN5 */
  &PORTC, &DDRC, PC5,		/* ch 5 - EN2 */
  &PORTC, &DDRC, PC0  /* ch 6 - EN4 */
};
    
#define TWI_PORT PORTD
#define TWI_DDR  DDRD

#define TWI_SDA_BIT PD4
#define TWI_SCL_BIT PD5


#endif


void laser_setup();
void laser_set_pot( uint8_t v);
void laser_read_adc( uint16_t* vals);
void laser_sel_chan( uint8_t c);

#endif
