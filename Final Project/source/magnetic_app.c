/***************************************************************************//**
  @file
  @brief
  @author
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "magnetic_app.h"
#include "magnetic.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

static uint8_t id[ID_SIZE+1];

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


void init_magnetic_ma(void){
	init_magnetic();
}
void enable_reading_ma(void){
	enable_reading();
}
void disable_reading_ma(void){
	disable_reading();
}
READING_STATE_MA get_state_ma(void){
	READING_STATE state = get_state();
	switch(state){
	case UNINITIALIZED_M:
		return UNINITIALIZED_MA;
		break;
	case DISABLED_M:
		return DISABLED_MA;
		break;
	case WAITING_M:
		return WAITING_MA;
		break;
	case READING_M:
		return READING_MA;
		break;
	case INVALID_DATA_M:
		return INVALID_DATA_MA;
		break;
	case NEW_VALID_DATA_M:
		return NEW_VALID_DATA_MA;
		break;
	}
	return ERROR_MA;
}
const uint8_t* get_data_ma(void){
	volatile const char* data = get_data();
	if(data != NULL){
		for(int i = 0; i<ID_SIZE; i++){
			id[i] = (data[i+1] & 0x0F) | 0x30;
		}
		id[ID_SIZE] = 0;
	}else{
		for(int i = 0; i<ID_SIZE+1; i++)
			id[i] = 0;
	}
	return id;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
