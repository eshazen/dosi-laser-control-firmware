bool i2c_error();
void I2C_delay(void);
bool read_SCL(void);
bool read_SDA(void);
void set_SCL(void);
void clear_SCL(void);
void set_SDA(void);
void clear_SDA(void);
void arbitration_lost(void);
void i2c_start_cond(void);
void i2c_stop_cond(void);
void i2c_write_bit(bool bit);
bool i2c_read_bit(void);
bool i2c_write_byte(bool send_start,
                    bool send_stop,
                    unsigned char byte);
unsigned char i2c_read_byte(bool nack, bool send_stop);
void i2c_begin();
