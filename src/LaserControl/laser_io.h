#ifndef LASER_IO_H
#define LASER_IO_H

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

// ---------- pinout for UNO for testing ----------
#ifdef UNO_PINS

// enables
static struct {
  volatile uint8_t *port;
  volatile uint8_t *ddr;
  uint8_t pin;
} enables[] = {
  &PORTD, &DDRD, PD7,		/* ch 1 - EN1 */
  &PORTD, &DDRD, PD6,		/* ch 2 - EN0 */
  &PORTD, &DDRD, PD5,		/* ch 3 - EN3 */
  &PORTD, &DDRD, PD4,		/* ch 4 - EN5 */
  &PORTD, &DDRD, PD3,		/* ch 5 - EN2 */
  &PORTD, &DDRD, PD2		/* ch 6 - EN4 */
};

#define TWI_PORT PORTC
#define TWI_PIN PINC
#define TWI_DDR  DDRC

#define TWI_SDA_BIT PC4
#define TWI_SCL_BIT PC5

#define I2C_RST_PORT PORTB
#define I2C_RST_DDR DDRB
#define I2C_RST_PIN PB0

#endif


// ---------- pinout for FMC ---------- 
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
#define TWI_PIN PIND
#define TWI_DDR  DDRD

#define TWI_SDA_BIT PD4
#define TWI_SCL_BIT PD5

#define I2C_RST_PORT PORTD
#define I2C_RST_DDR DDRD
#define I2C_RST_PIN PD7


#endif


void laser_setup();
bool laser_set_pot( uint8_t v);
bool laser_read_adc( uint16_t* vals);
bool laser_sel_chan( uint8_t c);
void laser_reset_i2c();
void laser_enable( uint8_t c, bool v);

#endif
