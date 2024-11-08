/***************************************************************************//**
  @file     gpio.h
  @brief    Simple GPIO Pin services, similar to Arduino
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _MAGNETIC_H_
#define _MAGNETIC_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

typedef enum {UNINITIALIZED_M,DISABLED_M,WAITING_M,READING_M,INVALID_DATA_M,NEW_VALID_DATA_M}READING_STATE;

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void init_magnetic(void);
void enable_reading(void);
void disable_reading(void);
READING_STATE get_state(void);
volatile const char* get_data(void); //llamar solo si get_sate devuelve NEW_VALID_DATA_M, sino, podria devolver NULL


/*******************************************************************************
 ******************************************************************************/

#endif
