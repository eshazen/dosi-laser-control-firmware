//
// laser I/O control
//
// laser_setup()            - call once to set up I/O
// laser_set_pot( v, a)     - set digital pot v=value 0-255  a=0 (volatile) 2 (non-volatile)
// laser_read_adc( &vals)   - read 8 x 16-bit values to &vals
// laser_sel_chan( c)       - set I2C mux to channel 0-5 corresponding to printed labels 1-6
// laser_reset_i2c()        - reset the I2C mux
// laser_enable( c, v)      - enable or disable a laser via discrete signal
// laser_leds( v)           - set on-board LEDs using bits 0, 1
//


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
  uint8_t sel;
} enables[] = {
  &PORTB, &DDRB, PB2, 1,	/* ch 1 - EN1 */
  &PORTB, &DDRB, PB0, 0,	/* ch 2 - EN0 */
  &PORTB, &DDRB, PB6, 3,	/* ch 3 - EN3 */
  &PORTC, &DDRC, PC2, 5,	/* ch 4 - EN5 */
  &PORTC, &DDRC, PC5, 2,	/* ch 5 - EN2 */
  &PORTC, &DDRC, PC0, 4         /* ch 6 - EN4 */
};
    
#define TWI_PORT PORTD
#define TWI_PIN PIND
#define TWI_DDR  DDRD

#define TWI_SDA_BIT PD4
#define TWI_SCL_BIT PD5

#define I2C_RST_PORT PORTD
#define I2C_RST_DDR DDRD
#define I2C_RST_PIN PD7

#define LED_DDR DDRD
#define LED1_BIT PD3
#define LED2_BIT PD6
#define LED_PORT PORTD

#endif


void laser_setup();
bool laser_set_pot( uint16_t v, uint8_t a);
bool laser_read_pot( uint16_t* vals);
bool laser_read_adc( uint16_t* vals);
bool laser_sel_chan( uint8_t c);
void laser_reset_i2c();
void laser_enable( uint8_t c, bool v);
void laser_leds( uint8_t v);

#endif
