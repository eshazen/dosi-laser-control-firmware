#ifndef I2C_BB_H_
#define I2C_BB_H_

// bit-bang I2C from:
// https://github.com/WifWaf/I2C_BIT_BANG/
// hacked to work with avr-libc
// converted to C

#include <stdbool.h>
#include <avr/io.h>
#include "../libs/ioport.h"
#include <util/delay.h>

#define delayMicroseconds(d) _delay_us(d)


#include <inttypes.h>

void i2c_begin();
        
void i2c_transmission_begin(uint8_t adr);
bool i2c_transmission_write(uint8_t *data, uint8_t len);
bool i2c_transmission_read(uint8_t *data, uint8_t len);
void i2c_transmission_end();

uint8_t i2c_get_u8();
void i2c_send_u8(uint8_t *data);

bool i2c_check_ack();
void i2c_send_ack();
void i2c_scl_tick();
void i2c_stop();
void i2c_start();

#endif // I2C_BB_H_
