//
// I2C functions for laser board
//

// #define DEBUG

// issue separate convert for each channel
#define SINGLE_CH_ADC

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "../libs/ioport.h"
#include "laser_io.h"
#include "i2c_wiki.h"

// data buffer
static uint8_t data[16];

extern char tmp[10];

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

// | reg sel2 sel1 sel0 | clk bip nrst x
// |  1    1    1    1  |  0   0   1   0     
#define ADC_SETUP 0xf2
// | reg scn1 scn0 cs3 | cs2 cs1 cs0 sgl |
// |  0   0    0    0  |  1   1   1   1  |   scan all channels
#define ADC_CONF_SCAN 0x0f
// |  0   1    1    c  |  c   c   c   1  |   convert one channel
#define ADC_CONF_SINGLE(c) (0x61|((c)<<1))

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
// values 0-0x100
bool laser_set_pot( uint16_t v, uint8_t a) {

  bool nack = false;
  uint8_t addr_byte = (a & 15) << 4;
  // check for MSB in data
  if( v & 0x100)
    addr_byte |= 1;

#ifdef DEBUG
  utoa( addr_byte, tmp, 16);
  puts( tmp);
#endif  

  //  i2c_transmission_begin( DPOT_ADDR);
  nack |= i2c_write_byte( true, false, DPOT_ADDR_W);
  // command byte including register address and MSB (bit 8)
  // a3 a2 a1 a0 0 0 x d8    (d8 is MSB)
  nack |= i2c_write_byte( false, false, addr_byte);
  // low 8 data bits
  nack |= i2c_write_byte( false, true, v & 0xff);

  return nack;
}

#ifdef DEBUG
void pnack( bool nack) {
  if( nack)
    putchar('N');
  else
    putchar('A');
}
#endif


//
// read digital pot (to 16 word buffer)
//
bool laser_read_pot( uint16_t* vals) {
  bool nack = false;
  uint8_t c0;
  uint8_t c1;
  
  // command byte including register address
  for( uint8_t a=0; a<16; a++) {
    nack |= i2c_write_byte( true, false, DPOT_ADDR_W);
#ifdef DEBUG
    pnack( nack);
#endif    
    // address = a3 a2 a1 a0 1 1 0 0    for write
    nack |= i2c_write_byte( false, false, ((a & 15) << 4) | 0xc );
#ifdef DEBUG
    pnack( nack);
#endif    
    nack |= i2c_write_byte( true, false, DPOT_ADDR_R);    
#ifdef DEBUG
    pnack( nack);
#endif    
    c1 = i2c_read_byte( false, false);
    c0 = i2c_read_byte( true, true);
    vals[a] = (c1 << 8) | c0;
#ifdef DEBUG
    putchar('|');
#endif    
  }

  return nack;
}

//
// read laser ADC, 8 channels
//
bool laser_read_adc( uint16_t* vals) {

  uint8_t c0;
  uint8_t c1;
  bool nack = false;

#ifdef SINGLE_CH_ADC

  // convert channels individually in a loop
  for( int i=0; i<8; i++) {
    nack |= i2c_write_byte( true, false, ADC_ADDR_W);
    nack |= i2c_write_byte( false, false, ADC_SETUP);
    nack |= i2c_write_byte( false, true, ADC_CONF_SINGLE(i));
  
    nack |= i2c_write_byte( true, false, ADC_ADDR_R);

    c1 = i2c_read_byte( false, false);
    c0 = i2c_read_byte( true, false);
    vals[i] = ((c1 & 15) << 8) | c0;
  }  

#else  

  // let the ADC convert channels on it's own
  // this gives funny results for some inputs, so don't use it!
  
  nack |= i2c_write_byte( true, false, ADC_ADDR_W);
  nack |= i2c_write_byte( false, false, ADC_SETUP);
  nack |= i2c_write_byte( false, true, ADC_CONF_SCAN);
  
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

#endif  

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
