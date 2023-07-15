#ifndef LASER_IO_H
#define LASER_IO_H

#define LASER_ENA_PORT PORTD
#define LASER_ENA_DDR DDRD

#define LASER_ENA_PIN PD7

int laser_set_pot( uint8_t v);
int laser_read_adc( uint16_t* vals);

#endif
