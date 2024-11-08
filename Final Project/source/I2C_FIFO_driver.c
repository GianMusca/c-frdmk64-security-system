#include "I2C_Driver.h"
#include "I2C_FIFO_driver.h"
#include "timer.h"
#include "stdio.h"

typedef struct{
	bool rnotw;
	uint32_t s_addr;
	uint8_t* w_q;
	uint8_t* r_q;
	uint8_t w_q_s;
	uint8_t r_q_s;
}I2C_FIFO_cell;

I2C_FIFO_cell I2C_FIFO[I2C_FIFO_SIZE];

static int I2C_FIFO_head = 0;
static int I2C_FIFO_tail = 0;
static tim_id_t I2C_FIFO_timer;

static void I2C_FIFO_send_next_message(void);
static void I2C_FIFO_end_routine_callback(void);
static bool I2C_is_sending_message;

static I2C_FIFO_callback I2C_FIFO_emptycallback = (void*)0;

void I2C_FIFO_init(void){
	static bool init = false;
	if(!init){
		I2C_FIFO_head = 0;
		I2C_FIFO_tail = 0;
		I2C_is_sending_message = false;
		timerInit();
		I2C_FIFO_timer = timerGetId();
		timerStart(I2C_FIFO_timer,10,TIM_MODE_PERIODIC,I2C_FIFO_send_next_message);
		I2C_Init();
		I2C_End_Routine(I2C_FIFO_end_routine_callback);
		init = true;
	}
}

void I2C_FIFO_new_message(bool rnotw, uint32_t s_addr, uint8_t* w_q, uint8_t* r_q, uint8_t w_q_s, uint8_t r_q_s){
	I2C_FIFO[I2C_FIFO_tail].rnotw = rnotw;
	I2C_FIFO[I2C_FIFO_tail].s_addr = s_addr;
	I2C_FIFO[I2C_FIFO_tail].w_q = w_q;
	I2C_FIFO[I2C_FIFO_tail].r_q = r_q;
	I2C_FIFO[I2C_FIFO_tail].w_q_s = w_q_s;
	I2C_FIFO[I2C_FIFO_tail].r_q_s = r_q_s;

	if(++I2C_FIFO_tail>=I2C_FIFO_SIZE)
		I2C_FIFO_tail = 0;
	if(I2C_FIFO_tail == I2C_FIFO_head)
		if(++I2C_FIFO_head>=I2C_FIFO_SIZE)
			I2C_FIFO_head=0;

	//if(!timerRunning(I2C_FIFO_timer))
	//	timerStart(I2C_FIFO_timer,10,TIM_MODE_PERIODIC,I2C_FIFO_send_next_message);
}

static void I2C_FIFO_send_next_message(void){
	if(!I2C_FIFO_is_empty()){
		I2C_is_sending_message = true;
		I2C_Communication_Config(
				I2C_FIFO[I2C_FIFO_head].rnotw,
				I2C_FIFO[I2C_FIFO_head].s_addr,
				I2C_FIFO[I2C_FIFO_head].w_q,
				I2C_FIFO[I2C_FIFO_head].r_q,
				I2C_FIFO[I2C_FIFO_head].w_q_s,
				I2C_FIFO[I2C_FIFO_head].r_q_s);
		if(++I2C_FIFO_head>=I2C_FIFO_SIZE)
			I2C_FIFO_head=0;
	}
}

void I2C_FIFO_empty_callback(I2C_FIFO_callback callback){
	I2C_FIFO_emptycallback = callback;
}

static void I2C_FIFO_end_routine_callback(void){
	if(I2C_FIFO_is_empty() && I2C_FIFO_emptycallback != (void*)0)
		I2C_FIFO_emptycallback();
}

bool I2C_FIFO_is_empty(void){
	return I2C_FIFO_head==I2C_FIFO_tail;
}
