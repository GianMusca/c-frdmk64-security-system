/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "header/gpio.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/***** BOARD defines **********************************************************/

// Digital IO
#define	DIO_0	PORTNUM2PIN(PC,16)
#define DIO_1	PORTNUM2PIN(PC,17)
#define DIO_2	PORTNUM2PIN(PB,9)
#define DIO_3	PORTNUM2PIN(PA,1)
#define DIO_4	PORTNUM2PIN(PB,23)
#define DIO_5	PORTNUM2PIN(PA,2)
#define DIO_6	PORTNUM2PIN(PC,2)
#define DIO_7	PORTNUM2PIN(PC,3)

//#define DIO_8	PORTNUM2PIN(PA,0)
#define DIO_9	PORTNUM2PIN(PC,4)
#define DIO_10	PORTNUM2PIN(PD,0)
#define DIO_11	PORTNUM2PIN(PD,2)
#define DIO_12	PORTNUM2PIN(PD,3)
#define DIO_13	PORTNUM2PIN(PD,1)
#define DIO_14	PORTNUM2PIN(PE,25)
#define DIO_15	PORTNUM2PIN(PE,24)

// Analog In
#define AI_0	PORTNUM2PIN(PB,2)
#define AI_1	PORTNUM2PIN(PB,3)
#define AI_2	PORTNUM2PIN(PB,10)
#define AI_3	PORTNUM2PIN(PB,11)
#define AI_4	PORTNUM2PIN(PC,11)
#define AI_5	PORTNUM2PIN(PC,10)


// On Board User LEDs
#define PIN_LED_RED     PORTNUM2PIN(PB,22) // PTB22
#define PIN_LED_GREEN   PORTNUM2PIN(PE,26) // PTE26
#define PIN_LED_BLUE    PORTNUM2PIN(PB,21) // PTB21
#define PIN_LED_YELLOW	PORTNUM2PIN(PB,23) // PTB23

#define LED_ACTIVE      LOW


// On Board User Switches
#define PIN_PULS        PORTNUM2PIN(PC, 9)// PTC9
#define PIN_SW2         PORTNUM2PIN(PC, 6)// PTC6
#define PIN_SW3         PORTNUM2PIN(PA, 4)// PTA4

#define SW_ACTIVE       LOW// ???
#define SW_INPUT_TYPE   // ???


/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
