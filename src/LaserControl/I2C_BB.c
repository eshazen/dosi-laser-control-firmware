// bit-bang I2C from:
// https://github.com/WifWaf/I2C_BIT_BANG/
// hacked to work with avr-libc
// converted to C

#include <avr/io.h>
#include <stdbool.h>
#include "I2C_BB.h"
#include "../libs/ioport.h"

static uint8_t _i2c_adr;

// Increased the duration of the SCL high (clock)
#define SCL_CLOCK_STRECH 10

// Generic timer, slows everything down (ueful on the ESP32 - replace with vTaskDelay)
#define DELAY_GENERIC 0

// Note I2C SDA is open drain, this means we need to use INPUT for an SDA high logic. I.e. we can't just write digitalWrite(pin, HIGH) for the SDA.
#define SDA_HIGH clear_io_bits( &TWI_DDR, _BV(TWI_SDA_BIT)); delayMicroseconds(DELAY_GENERIC);
#define SDA_LOW  set_io_bits( &TWI_DDR, _BV(TWI_SDA_BIT)); delayMicroseconds(DELAY_GENERIC);

// SCL is straight forward, and provides the clock - it provides a signal to read/write each bit.
#define SCL_HIGH set_io_bits( &TWI_PORT, _BV( TWI_SCL_BIT)); delayMicroseconds(DELAY_GENERIC);
#define SCL_LOW clear_io_bits(&TWI_PORT, _BV( TWI_SCL_BIT)); delayMicroseconds(DELAY_GENERIC);

// read SDA
#define READ_SDA() get_io_bits(&TWI_PORT,_BV(TWI_SDA_BIT))

// This simply shifts the bits one space to the left and adds the write/read bit to the 7 bit address (device address commonly used). 
// E.g. For a read command, the address 0x01, which is 0000 0001 in binary would become 0000 0011 or 0x03.
#define ADR7_AS_WRITE8(A) do { (A) <<= 1; (A) &= 0xfe; } while(0)
#define ADR7_AS_READ8(A) do { (A) <<= 1; (A) |= 0x01; } while(0)

// Sequences --------------------------------------------------- //
void i2c_begin()
{
  //  pinMode(_scl, OUTPUT);  // SCL is digital, so set it
  set_io_bits( &TWI_DDR, _BV(TWI_SCL_BIT));
  clear_io_bits( &TWI_PORT, _BV(TWI_SDA_BIT)); // turn off pull-up

  SDA_HIGH;               // place bus in initial logic state
  SCL_HIGH;
}

void i2c_transmission_begin(uint8_t adr)       // Begin new transmission
{
  _i2c_adr = adr;        // Copy the address to a local var
  i2c_start();           // Send the start sequence
}

bool i2c_transmission_write(uint8_t *data, uint8_t len)     // Write byte(s) to I2C - called after begin
{
  ADR7_AS_WRITE8(_i2c_adr);            // Add the write bit and shift address left 1
  i2c_send_u8(&_i2c_adr);                  // Send address with the read/write bit over bus
  uint8_t ret = i2c_check_ack();       // Let's check for the acknowledgement bit, was our data recieved?

  for(uint8_t i = 0; i < len; i++)  // If not, don't send data and check acknowledgement, otherwise, send as much as we need (NOTE - no stop command)
  {
    if(ret)
      break;
   
    i2c_send_u8(&data[i]);
    ret = i2c_check_ack();
  }

  return ret;                       // Return a bool representing sucess or failure
}

bool i2c_transmission_read(uint8_t *data, uint8_t len)   // Read byte(s) from I2C - called after begin
{
  ADR7_AS_READ8(_i2c_adr);                // 'Add' the read bit (we set it to 0), and shift address left 1
  i2c_send_u8(&_i2c_adr);                     // Send address with read/write bit over bus
  uint8_t ret = i2c_check_ack();          // Let's check for the acknowledgement bit, was our data recieved?
  
  if(!ret)                             // if not, don't read data, otherwise read according to the length given
  {
    for(uint8_t i = 0; i < len; i++)
    {
      data[i] = i2c_get_u8();
      i2c_send_ack();                      // We often need to send an acknowledgement that we recieved the data here
    }
  }

  return ret;                           // return failure or sucess
}

void i2c_transmission_end()          // End transmission 
{
  _i2c_adr = 0;                         //  return deive address to 0, for new tranmission
  i2c_stop();                                 // Send stop sequence over I2C, relaying with have finished
}

// IO -------------------------------------------------------- //

uint8_t i2c_get_u8()          // Get (e.g. read) a byte over I2C
{
  uint8_t buff = 0x00;             // A buffer for our data
  // A loop for the reading each bit from SDA. Scl_tick signals we are finished and/or ready for the next bit
  for(uint8_t i = 0; i < 8; i++)       
  { 
    // Data comes in MSB to LSB, so we need to move it over to the left. 
    // I.e. 0x01 which is 0000 0001 becomes 1000 0000 by left shifting << 7 times
    // This is reduced on each loop for positioning. 
    //    buff |= ((digitalRead(_sda) << (7-i)));
    buff |= ((READ_SDA() << (7-i)));

    // Done here signal, next
    i2c_scl_tick();
  }  

  // Return the byte we've reconstructed.   
  return buff;
}

void i2c_send_u8(uint8_t *data)       // Send a byte over I2C
{
  for(uint8_t i = 0; i < 8; i++)          // 8 bits, so 8 loops
  { 
    if((0x80 >> i) & *data)               // Check if i bit to be sent is a 1 or 0
    {    
      SDA_HIGH;                           // It's a 1, so send a logic HIGH
    }
    else
    {
      SDA_LOW;                            // It's a 0, so just pretend we need send a 0 for completion
    }
    i2c_scl_tick();                           // Signal that we sent the logic
    SDA_LOW;                              //  pretend we need to return back to LOW  either way
  }
}

// Protocol -------------------------------------------------- //

bool i2c_check_ack()                  // Check for the akcnowledgment bit
{
  uint8_t ret = 0;                         // buffer for failure / success variable

  SDA_HIGH;                                // Raise SDA to logic HIGH
  SCL_HIGH;                                // Raise SCL to logic HIGH (essenitally another scl_tick()) - we need that acknowledgment bit.
  
  _delay_us(SCL_CLOCK_STRECH);      // Stretch high time  
  //  ret = digitalRead(_sda);                  // What is SDA now?
  ret = READ_SDA();                  // What is SDA now?
  
  SCL_LOW;                                  // Return SCL to normal, essentially an scl_tick complete

  return ret;                               // return acknowlegement bit to signal success or failure
}

void i2c_send_ack()     // send acknowledgement bit
{
  SDA_LOW;                  // Send a tick to signal acknowledgement bit is ready, while pulling SDA low (acknowledged posiition)
  i2c_scl_tick();
  SDA_HIGH;
}

void i2c_scl_tick()                     // SCL clock, i.e. signal that data is ready/required
{
  SCL_HIGH;                                 // Just making a tooth shape, raise SCL then lower
  delayMicroseconds(SCL_CLOCK_STRECH);
  SCL_LOW;
}

void i2c_stop()                 // Stop sequence - we have finished sending/recieving data
{
  SDA_LOW;                           // As required by the protocol
  SCL_HIGH;
  SDA_HIGH;
}

void i2c_start()              // Start sequence - start trading bits
{
  SDA_LOW;                         // As described by the protocol
  SCL_LOW;
}
