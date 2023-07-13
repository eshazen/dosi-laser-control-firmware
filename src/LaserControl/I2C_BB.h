#ifndef I2C_BB_H_
#define I2C_BB_H_

// bit-bang I2C from:
// https://github.com/WifWaf/I2C_BIT_BANG/
// hacked to work with avr-libc

// some Arduino fudge macros
#include <avr/io.h>
#include "../libs/ioport.h"
#include <util/delay.h>

#define delayMicroseconds(d) _delay_us(d)

// define the pins the AVR way
#define TWI_PORT PORTC
#define TWI_DDR  DDRC

#define TWI_SDA_BIT PC4
#define TWI_SCL_BIT PC5


#include <inttypes.h>

class i2c_bb
{
    public:
        void begin(uint8_t sda = 2, uint8_t scl = 3);
        
        void transmission_begin(uint8_t adr);
        bool transmission_write(uint8_t *data, uint8_t len);
        bool transmission_read(uint8_t *data, uint8_t len);
        void transmission_end();
    protected:
    private:
        uint8_t _sda;
        uint8_t _scl;
        uint8_t _adr;

        uint8_t get_u8();
        void send_u8(uint8_t *data);

        bool check_ack();
        void send_ack();
        void scl_tick();
        void stop();
        void start();
};




#endif // I2C_BB_H_
