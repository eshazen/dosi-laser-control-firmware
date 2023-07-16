
// data buffer
static uint8_t data[16];

// D-POT I2C address with A0 = 0
#define DPOT_ADDR 0x2E

// ADC I2C address / regs
#define ADC_ADDR 0x33
#define ADC_SETUP 0xd2
#define ADC_CONF 0x0f

// PCA9547 I2C Mux address = 111 0 aaa 
#define MUX_ADDR 0x70

//
// set I2C mux to channel
//
void laser_sel_chan( uint8_t c) {
  i2c_transmission_begin( MUX_ADDR);
  data[0] = 0x10 | c;
  i2c_transmission_write( data, 1);
}

//
// set digital pot
//
void laser_set_pot( uint8_t v) {

  i2c_transmission_begin( DPOT_ADDR);
  data[0] = 0;			/* control byte for register 0 */
  data[1] = v;
  i2c_transmission_write( data, 2);
  i2c_transmission_end();
}

//
// read laser ADC, 8 channels
//
void laser_read_adc( uint16_t* vals) {

  i2c_transmission_begin( ADC_ADDR);
  data[0] = ADC_SETUP;
  data[1] = ADC_CONF;
  i2c_transmission_write( data, 2);

  // read raw data
  i2c_transmission_read( data, 16);
  // copy to buffer
  for( int i=0; i<8; i++)
    vals[i] = ((data[2*i] & 0xf) << 8) | data[2*i+1];
}
