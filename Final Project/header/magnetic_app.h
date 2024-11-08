/***************************************************************************//**
  @file     gpio.h
  @brief    Simple GPIO Pin services, similar to Arduino
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _MAGNETIC_APP_H_
#define _MAGNETIC_APP_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ID_SIZE 8
typedef enum {UNINITIALIZED_MA,DISABLED_MA,WAITING_MA,READING_MA,INVALID_DATA_MA,NEW_VALID_DATA_MA,ERROR_MA}READING_STATE_MA;
//				Rojo			Verde		Amarillo	Azul		Magenta			Cian			Blanco


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void init_magnetic_ma(void);
void enable_reading_ma(void);
void disable_reading_ma(void);
READING_STATE_MA get_state_ma(void);
const uint8_t* get_data_ma(void);


/*******************************************************************************
 ******************************************************************************/

#endif
