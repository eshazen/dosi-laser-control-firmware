#ifndef LASER_IO_H
#define LASER_IO_H

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

// number the enables.  This is really confusing as the PCB number doesn't
// match the net names.  Thusly:
//   ch 1 = EN1  ch 2 = EN0  ch 3 = EN3  ch 4 = EN5  ch 5 = EN2  ch 6 = EN4

// ch 1 = EN1
#define CH1_PORT PORTB
#define CH1_PIN PB2

// ch 2 = EN0
#define CH2_PORT PORTB
#define CH2_PIN PB0

//
#define CH3_PORT 
#define CH3_PIN 

#define CH4_PORT 
#define CH4_PIN 

#define CH5_PORT 
#define CH5_PIN 

#define CH6_PORT 
#define CH6_PIN 

#define TWI_PORT PORTD
#define TWI_DDR  DDRD

#define TWI_SDA_BIT PD4
#define TWI_SCL_BIT PD5


#endif



void laser_set_pot( uint8_t v);
void laser_read_adc( uint16_t* vals);
void laser_sel_chan( uint8_t c);

#endif
