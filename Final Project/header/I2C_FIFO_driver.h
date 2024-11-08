#include <stdbool.h>

#define I2C_FIFO_SIZE 16

typedef void (*I2C_FIFO_callback)(void);

void I2C_FIFO_init(void);
bool I2C_FIFO_is_empty(void);
void I2C_FIFO_empty_callback(I2C_FIFO_callback callback);
void I2C_FIFO_new_message(bool rnotw, uint32_t s_addr, uint8_t* w_q, uint8_t* r_q, uint8_t w_q_s, uint8_t r_q_s);
