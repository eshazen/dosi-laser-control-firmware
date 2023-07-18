
#include <stdio.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "../libs/ioport.h"
#include "laser_io.h"
#include "i2c_wiki.h"

// data buffer
static uint8_t data[16];

#define ADDR_W(a) ((a)<<1)
#define ADDR_R(a) (ADDR_W(a)|1)

// D-POT I2C address with A0 = 0
#define DPOT_ADDR 0x2E
#define DPOT_ADDR_W ADDR_W(DPOT_ADDR)
#define DPOT_ADDR_R ADDR_R(DPOT_ADDR)

// ADC I2C address / regs
#define ADC_ADDR 0x33
#define ADC_ADDR_W ADDR_W(ADC_ADDR)
#define ADC_ADDR_R ADDR_R(ADC_ADDR)

#define ADC_SETUP 0xd2
#define ADC_CONF 0x0f

// PCA9547 I2C Mux address = 111 0aaa 
#define MUX_ADDR 0x70
#define MUX_ADDR_W ADDR_W(MUX_ADDR)
#define MUX_ADDR_R ADDR_R(MUX_ADDR)

//
// setup I/O
//
void laser_setup() {
  for( uint8_t i=0; i<6; i++) {
    // set enable DDR to output
    set_io_bits( enables[i].ddr, _BV( enables[i].pin));
    // set enables to LOW
    clear_io_bits( enables[i].port, _BV( enables[i].pin));
  }
  // set I2C mux nRST as output
  set_io_bits( &I2C_RST_DDR, _BV(I2C_RST_PIN));
  // set I2C mux nRST high (inactive)
  set_io_bits( &I2C_RST_PORT, _BV(I2C_RST_PIN));
  // set LED pins as output and turn off
  set_io_bits( &LED_DDR, _BV(LED1_BIT) | _BV(LED2_BIT));
  clear_io_bits( &LED_PORT, _BV(LED1_BIT) | _BV(LED2_BIT));
}


//
// set I2C mux to channel
//
bool laser_sel_chan( uint8_t c) {
  bool nack = false;
#ifdef FMC_PINS
  nack |= i2c_write_byte( true, false, MUX_ADDR_W);
  nack |= i2c_write_byte( false, true, 8 | enables[c].sel);
#endif
  return nack;
}

//
// set digital pot
//
bool laser_set_pot( uint8_t v, uint8_t a) {

  bool nack = false;
  //  i2c_transmission_begin( DPOT_ADDR);
  nack |= i2c_write_byte( true, false, DPOT_ADDR_W);
  // command byte including register address
  // a3 a2 a1 a0 0 0 0 0
  nack |= i2c_write_byte( false, false, (a & 15) << 4);
  nack |= i2c_write_byte( false, true, v);

  return nack;
}

//
// read laser ADC, 8 channels
//
bool laser_read_adc( uint16_t* vals) {

  uint8_t c0;
  uint8_t c1;
  bool nack = false;

  nack |= i2c_write_byte( true, false, ADC_ADDR_W);
  nack |= i2c_write_byte( false, false, ADC_SETUP);
  nack |= i2c_write_byte( false, true, ADC_CONF);
  
  if( nack)
    return nack;

  nack = false;

  nack |= i2c_write_byte( true, false, ADC_ADDR_R);
  for( int i=0; i<8; i++) {
    c1 = i2c_read_byte( false, false);
    if( i == 7)
      c0 = i2c_read_byte( true, false);
    else
      c0 = i2c_read_byte( false, false);
    vals[i] = ((c1 & 15) << 8) | c0;
  }    

  return nack;
}

//
// pulse reset on I2C mux low
//
void laser_reset_i2c() {
  clear_io_bits( &I2C_RST_PORT, _BV(I2C_RST_PIN));
  _delay_us( 10);
  set_io_bits( &I2C_RST_PORT, _BV(I2C_RST_PIN));
  _delay_us( 10);
}

//
// set enable on specified laser
// c = laser# 0..5
void laser_enable( uint8_t c, bool v) {
  if( c > 5)
    return;
  
  if( v)
    set_io_bits( enables[c].port, _BV( enables[c].pin));
  else
    clear_io_bits( enables[c].port, _BV( enables[c].pin));
}

//
// set LEDs for debug
// bit 0 - LED1  bit 1 - LED2
void laser_leds( uint8_t v) {
  uint8_t t = 0;
  // turn off first
  clear_io_bits( &LED_PORT, _BV(LED1_BIT) | _BV(LED2_BIT));
  if( v & 1)
    t |= _BV(LED1_BIT);
  if( v & 2)
    t |= _BV(LED2_BIT);
  set_io_bits( &LED_PORT, t);
}
