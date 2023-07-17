//
// Wikipedia I2C code (literally!)
// what the heck, it works :)
//
// TODO:  add timeouts
//

// #define DEBUG

#include <stdbool.h>
#include <stdio.h>

#include <avr/pgmspace.h>
#include <util/delay.h>

#include "laser_io.h"
#include "i2c_wiki.h"

#include "../libs/ioport.h"

// Hardware-specific support functions that MUST be customized:
static bool started = false; // global data
static bool arb_lost = false;

#ifdef DEBUG
static int txn_count = 0;
#endif

// delay in us
#define I2CSPEED 100

void i2c_begin()
{
  // ensure output registers are low
  clear_io_bits( &TWI_PORT, _BV(TWI_SDA_BIT));
  clear_io_bits( &TWI_PORT, _BV(TWI_SCL_BIT));

  // start with bits high
  set_SCL();
  set_SDA();
}

bool i2c_error(void) {
  if( arb_lost) {
    arb_lost = false;
    return true;
  }
  return false;
}

bool read_SCL(void) {  // Return current level of SCL line, 0 or 1
  return (get_io_bits(&TWI_PIN,_BV(TWI_SCL_BIT))!=0);
}

bool read_SDA(void) {  // Return current level of SDA line, 0 or 1
  return (get_io_bits(&TWI_PIN,_BV(TWI_SDA_BIT))!=0);
}
void set_SCL(void) {   // Do not drive SCL (set pin high-impedance)
  clear_io_bits( &TWI_DDR, _BV(TWI_SCL_BIT));
}

void clear_SCL(void) { // Actively drive SCL signal low
  set_io_bits( &TWI_DDR, _BV(TWI_SCL_BIT));
}

void set_SDA(void) {   // Do not drive SDA (set pin high-impedance)
  clear_io_bits( &TWI_DDR, _BV(TWI_SDA_BIT));
}

void clear_SDA(void) { // Actively drive SDA signal low
  set_io_bits( &TWI_DDR, _BV(TWI_SDA_BIT));
}

void arbitration_lost(void) {
  arb_lost = true;
}

void i2c_start_cond(void)
{
  if (started) { 
    // if started, do a restart condition
    // set SDA to 1
    set_SDA();
    I2C_delay();
    set_SCL();
    while (read_SCL() == 0) { // Clock stretching
      // You should add timeout to this loop
    }

    // Repeated start setup time, minimum 4.7us
    I2C_delay();
  }

  if (read_SDA() == 0) {
    arbitration_lost();
  }

  // SCL is high, set SDA from 1 to 0.
  clear_SDA();
  I2C_delay();
  clear_SCL();
  started = true;
}

void i2c_stop_cond(void)
{
  // set SDA to 0
  clear_SDA();
  I2C_delay();

  set_SCL();
  // Clock stretching
  while (read_SCL() == 0) {
    // add timeout to this loop.
  }

  // Stop bit setup time, minimum 4us
  I2C_delay();

  // SCL is high, set SDA from 0 to 1
  set_SDA();
  I2C_delay();

  if (read_SDA() == 0) {
    arbitration_lost();
  }

  started = false;
}

// Write a bit to I2C bus
void i2c_write_bit(bool bit)
{
  if (bit) {
    set_SDA();
  } else {
    clear_SDA();
  }

  // SDA change propagation delay
  I2C_delay();

  // Set SCL high to indicate a new valid SDA value is available
  set_SCL();

  // Wait for SDA value to be read by target, minimum of 4us for standard mode
  I2C_delay();

  while (read_SCL() == 0) { // Clock stretching
    // You should add timeout to this loop
  }

  // SCL is high, now data is valid
  // If SDA is high, check that nobody else is driving SDA
  if (bit && (read_SDA() == 0)) {
    arbitration_lost();
  }

  // Clear the SCL to low in preparation for next change
  clear_SCL();
}

// Read a bit from I2C bus
bool i2c_read_bit(void)
{
  bool bit;

  // Let the target drive data
  set_SDA();

  // Wait for SDA value to be written by target, minimum of 4us for standard mode
  I2C_delay();

  // Set SCL high to indicate a new valid SDA value is available
  set_SCL();

  while (read_SCL() == 0) { // Clock stretching
    // You should add timeout to this loop
  }

  // Wait for SDA value to be written by target, minimum of 4us for standard mode
  I2C_delay();

  // SCL is high, read out bit
  bit = read_SDA();

  // Set SCL low in preparation for next operation
  clear_SCL();

  return bit;
}

// Write a byte to I2C bus. Return 0 if ack by the target.
bool i2c_write_byte(bool send_start,
                    bool send_stop,
                    unsigned char byte)
{
  unsigned bit;
  bool     nack;

  if (send_start) {
    i2c_start_cond();
  }

  for (bit = 0; bit < 8; ++bit) {
    i2c_write_bit((byte & 0x80) != 0);
    byte <<= 1;
  }

  nack = i2c_read_bit();

  if (send_stop) {
    i2c_stop_cond();
  }

#ifdef DEBUG
  if( nack)
    printf("%d wb NACK\n", txn_count);
  else
    printf("%d wb ACK\n", txn_count);
  ++txn_count;
#endif  

  return nack;
}

// Read a byte from I2C bus
unsigned char i2c_read_byte(bool nack, bool send_stop)
{
  unsigned char byte = 0;
  unsigned char bit;

  for (bit = 0; bit < 8; ++bit) {
    byte = (byte << 1) | i2c_read_bit();
  }

  i2c_write_bit(nack);

  if (send_stop) {
    i2c_stop_cond();
  }

  return byte;
}

void I2C_delay(void)
{ 
  _delay_us(I2CSPEED);
}
