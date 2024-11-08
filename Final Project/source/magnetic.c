/***************************************************************************//**
  @file
  @brief
  @author
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"
#include "magnetic.h"
#include "board.h"
#include <stdbool.h>
#include <stdio.h>

#include "my_semaphores.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define data_pin   PORTNUM2PIN(PB,18)
#define clock_pin  PORTNUM2PIN(PB,19)
#define enable_pin PORTNUM2PIN(PC,9)//PORTNUM2PIN(PC,1)

#define CONTENT_MAX_SIZE 100

#define SS_CHAR ';'
#define FS_CHAR '='
#define ES_CHAR '?'

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static READING_STATE reading_state = UNINITIALIZED_M;
static bool reading = false;
static char content[CONTENT_MAX_SIZE+1];
static int content_index = 0;

static OS_ERR os_err;

typedef enum {NO_ERROR,UNFINISHED_MESSAGE,DATA_AFTER_FINISH_CHARACTER,NOT_VALID_EVENT,SS_EXPECTED,LRC_EXPECTED,PARITY_ERROR,BAD_LRC}ERROR_TYPE;
static ERROR_TYPE error = NO_ERROR;

typedef enum {NONE,ENABLE,DISABLE,LOGIC0,LOGIC1,ERROR_EVENT,NUMBER,SS,FS,ES,OTHER} EVENT;
typedef struct state_diagram_edge STATE;
struct state_diagram_edge{
	EVENT event;
	STATE *next_state;
	void(*p_action_fun)(void);
};

void add_1(void);
void add_0(void);
void do_nothing(void);
void clean_message(void);
void clean_current_character(void);
void add_character_to_content(void);
void reset_FSM(void);
void begin_SS(void);
void validate_LRC(void);
void finished_early(void);
void data_after_LRC(void);
void event_not_valid(void);
void SS_expected(void);
void LRC_expected(void);
void finish_reading(void);
EVENT character2event(void);

void add_data_to_current_character(bool bit);

void clock_falling_edge(void);
void enable_changed_state(void);

uint8_t character2char(void);


void fsm_handler(EVENT event);
bool current_character_valid_parity(void);

static bool initialized = false;
static bool data_valid  = false;


typedef struct{
	uint8_t data       :5;
	uint8_t loaded_bits:3;
} transmitted_character;

transmitted_character current_character;



extern STATE initial_state[];
extern STATE enabled_state[];
extern STATE expecting_SS_state[];
extern STATE pan_state[];
extern STATE additional_data_state[];
extern STATE expecting_LRC_state[];
extern STATE expecting_disable_state[];
extern STATE error_state[];

static STATE *current_state = initial_state;

STATE initial_state[]={
		{NONE,	   initial_state,do_nothing},
		{ENABLE,   enabled_state,clean_message},
		{DISABLE,  initial_state,do_nothing   },
		{LOGIC0,   initial_state,do_nothing   },
		{LOGIC1,   initial_state,do_nothing   },
		{ERROR_EVENT,error_state,event_not_valid}
};
STATE enabled_state[]={
		{NONE,    enabled_state,     do_nothing},
		{ENABLE,  enabled_state,     do_nothing},
		{DISABLE, initial_state,     finished_early},
		{LOGIC0,  enabled_state,     do_nothing},
		{LOGIC1,  expecting_SS_state,begin_SS  },
		{ERROR_EVENT,error_state,event_not_valid}

};
STATE expecting_SS_state[]={
		{NONE,     expecting_SS_state,do_nothing},
		{ENABLE,   expecting_SS_state,do_nothing},
		{DISABLE,  initial_state,     finished_early},
		{LOGIC0,   expecting_SS_state,add_0     },
		{LOGIC1,   expecting_SS_state,add_1     },
		{ERROR_EVENT,error_state,event_not_valid},
		{NUMBER, error_state,SS_expected},
		{SS,pan_state,add_character_to_content},
		{FS,error_state,SS_expected},
		{ES,error_state,SS_expected},
		{OTHER,error_state,SS_expected}
};
STATE pan_state[]={
		{NONE,     pan_state,    do_nothing},
		{ENABLE,   pan_state,    do_nothing},
		{DISABLE,  initial_state,finished_early},
		{LOGIC0,   pan_state,    add_0     },
		{LOGIC1,   pan_state,    add_1     },
		{ERROR_EVENT,error_state,event_not_valid},
		{NUMBER,pan_state,add_character_to_content},
		{SS,pan_state,add_character_to_content},
		{FS,additional_data_state,add_character_to_content},
		{ES,pan_state,add_character_to_content},
		{OTHER,pan_state,add_character_to_content}
};
STATE additional_data_state[]={
		{NONE,     additional_data_state,do_nothing},
		{ENABLE,   additional_data_state,do_nothing},
		{DISABLE,  initial_state,        finished_early},
		{LOGIC0,   additional_data_state,add_0     },
		{LOGIC1,   additional_data_state,add_1     },
		{ERROR_EVENT,error_state,event_not_valid},
		{NUMBER,additional_data_state,add_character_to_content},
		{SS,additional_data_state,add_character_to_content},
		{FS,additional_data_state,add_character_to_content},
		{ES,expecting_LRC_state,add_character_to_content},
		{OTHER,additional_data_state,add_character_to_content}
};
STATE expecting_LRC_state[]={
		{NONE,   expecting_LRC_state,do_nothing},
		{ENABLE, expecting_LRC_state,do_nothing},
		{DISABLE,initial_state,      finished_early},
		{LOGIC0, expecting_LRC_state,add_0      },
		{LOGIC1, expecting_LRC_state,add_1      },
		{ERROR_EVENT,error_state,event_not_valid},

		{NUMBER,expecting_disable_state,validate_LRC},
		{SS,expecting_disable_state,validate_LRC},
		{FS,expecting_disable_state,validate_LRC},
		{ES,expecting_disable_state,validate_LRC},
		{OTHER,expecting_disable_state,validate_LRC}

};
STATE expecting_disable_state[]={
		{NONE,     expecting_disable_state,do_nothing    },
		{ENABLE,   expecting_disable_state,do_nothing    },
		{DISABLE,  initial_state,          finish_reading },
		{LOGIC0,   expecting_disable_state,do_nothing    },
		{LOGIC1,   error_state,            data_after_LRC},
		{ERROR_EVENT,error_state,event_not_valid}
};
STATE error_state[]={
		{NONE,     error_state,   do_nothing},
		{ENABLE,   error_state,   do_nothing},
		{DISABLE,  initial_state, do_nothing},
		{LOGIC0,   error_state,   do_nothing},
		{LOGIC1,   error_state,   do_nothing},
		{ERROR_EVENT,error_state,event_not_valid}
};

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void init_magnetic(void){
	if(!initialized){

		gpioMode(data_pin,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioMode(clock_pin,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioIRQ(clock_pin,GPIO_IRQ_MODE_FALLING_EDGE,clock_falling_edge);
		gpioMode(enable_pin,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioIRQ(enable_pin,GPIO_IRQ_MODE_BOTH_EDGES,enable_changed_state);

		error = NO_ERROR;
		reset_FSM();
		reading_state = DISABLED_M;

		reading = false;
		initialized = true;
	}
}

void enable_reading(void){
	error = NO_ERROR;
	reset_FSM();

	reading_state = WAITING_M;
	reading = true;
}

void disable_reading(void){
	reading_state = DISABLED_M;
	reading = false;
}

READING_STATE get_state(void){
	return reading_state;
}

volatile const char* get_data(void){
	if(initialized && (error == NO_ERROR)){
		return content;
	}else{
		return NULL;
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void clock_falling_edge(void){
	if(reading){
		//gpioWrite(PIN_LED_GREEN,0);
		if(!gpioRead(data_pin))
			fsm_handler(LOGIC1);
		else
			fsm_handler(LOGIC0);
		//gpioWrite(PIN_LED_GREEN,1);
	}
}

void enable_changed_state(void){
	if(reading){
		//gpioWrite(PIN_LED_GREEN,0);
		if(gpioRead(enable_pin))
			fsm_handler(DISABLE);
		else
			fsm_handler(ENABLE);
		//gpioWrite(PIN_LED_GREEN,1);
	}
}

void do_nothing(void){}

void clean_message(void){
	for(int i=0; i<CONTENT_MAX_SIZE+1;i++)
		content[i]='\0';
	data_valid = false;
	error = NO_ERROR;
	content_index = 0;
	reading_state = READING_M;
	//gpioWrite(PIN_LED_GREEN,1);

	OSSemPost(&magnetic_updated_sem,OS_OPT_POST_1,&os_err);

}
void clean_current_character(void){
	current_character.data = 0;
	current_character.loaded_bits = 0;
}

void reset_FSM(void){
	clean_message();
	clean_current_character();
	current_state = initial_state;

	data_valid = false;

	content_index = 0;
}

void begin_SS(void){
	clean_current_character();
	add_1();
}

void add_1(void){
	add_data_to_current_character(1);
}

void add_0(void){
	add_data_to_current_character(0);
}

void add_data_to_current_character(bool bit){
	current_character.data |= bit<<current_character.loaded_bits;
	current_character.loaded_bits++;
	if (current_character.loaded_bits >= 5){
		if(current_character_valid_parity()){
			fsm_handler(character2event());
		}else{
			error = PARITY_ERROR;
			reading_state = INVALID_DATA_M;
			current_state = error_state;
		}
	}
}
void validate_LRC(void){
	uint8_t LRC = current_character.data;
	uint8_t xor_char = 0;
	for (int i = 0; i<content_index;i++)
		xor_char ^= content[i];
	if((uint8_t)(LRC & 0x0F) == (uint8_t)(xor_char & 0x0F))
		add_character_to_content();
	else{
		error = BAD_LRC;
		current_state = error_state;
		reading_state = INVALID_DATA_M;

		OSSemPost(&magnetic_updated_sem,OS_OPT_POST_1,&os_err);
	}

}
void finish_reading(void){
	data_valid = true;
	reading_state = NEW_VALID_DATA_M;
	//gpioWrite(PIN_LED_GREEN,0);

	OSSemPost(&magnetic_updated_sem,OS_OPT_POST_1,&os_err);
}

void finished_early(void){
	error = UNFINISHED_MESSAGE;
	reading_state = INVALID_DATA_M;

	OSSemPost(&magnetic_updated_sem,OS_OPT_POST_1,&os_err);
}
void data_after_LRC(void){
	error = DATA_AFTER_FINISH_CHARACTER;
	reading_state = INVALID_DATA_M;

	OSSemPost(&magnetic_updated_sem,OS_OPT_POST_1,&os_err);
}
void event_not_valid(void){
	error = NOT_VALID_EVENT;
	reading_state = INVALID_DATA_M;

	OSSemPost(&magnetic_updated_sem,OS_OPT_POST_1,&os_err);
}
void SS_expected(void){
	error = SS_EXPECTED;
	reading_state = INVALID_DATA_M;

	OSSemPost(&magnetic_updated_sem,OS_OPT_POST_1,&os_err);
}
void LRC_expected(void){
	error = LRC_EXPECTED;
	reading_state = INVALID_DATA_M;

	OSSemPost(&magnetic_updated_sem,OS_OPT_POST_1,&os_err);
}
void add_character_to_content(void){
	content[content_index++] = character2char();
	clean_current_character();
}
bool current_character_valid_parity(void){
	uint8_t data =current_character.data;
	return (bool)(data&(1<<0)) ^ (bool)(data&(1<<1)) ^ (bool)(data&(1<<2)) ^ (bool)(data&(1<<3)) ^ (bool)(data&(1<<4));
}

EVENT character2event(void){
	uint8_t character = character2char();
	if (character >= '0' && character <= '9')
		return NUMBER;
	else if (character == SS_CHAR)
		return SS;
	else if (character == FS_CHAR)
		return FS;
	else if (character == ES_CHAR)
		return ES;
	else
		return OTHER;
}

uint8_t character2char(void){
	return ((uint8_t)current_character.data & 0x0F) | 0x30;
}

void fsm_handler(EVENT event){
	void (*fun)(void) = current_state[event].p_action_fun;
	current_state = current_state[event].next_state;
	(*fun)();
}

/*******************************************************************************
 ******************************************************************************/
