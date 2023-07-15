
static uint8_t data[4];

// D-POT I2C address with A0 = 0
#define DPOT_ADDR 0x2E

// ADC I2C address
#define ADC_ADDR 0x33
#define ADC_SETUP 0xd2
#define ADC_CONF 0x0f

//
// set digital pot
//
int laser_set_pot( uint8_t v) {

  i2c_transmission_begin( DPOT_ADDR);
  data[0] = 0;			/* control byte for register 0 */
  data[1] = v;
  i2c_transmission_write( data, 2);
  i2c_transmission_end();
}

//
// read laser ADC, 8 channels
//
int laser_read_adc( uint16_t* vals) {

  i2c_transmission_begin( ADC_ADDR);
  data[0] = ADC_SETUP;
  data[1] = ADC_CONF;
  i2c_transmission_write( data, 2);

  // read 8 words
  
  
}
