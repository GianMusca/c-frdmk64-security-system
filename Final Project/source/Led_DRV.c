/***************************************************************************//**
  @file     +Nombre del archivo (ej: template.c)+
  @brief    +Descripcion del archivo+
  @author   +Nombre del autor (ej: Salvador Allende)+
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "Led_DRV.h"
#include "gpio.h"
#include "board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: unsigned int anio_actual;+
static bool initialized = false;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static void falta_envido (int);+


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
#define LED_G	PIN_LED_GREEN
#define LED_B	PIN_LED_BLUE
#define LED_R	PIN_LED_RED

#define ON	LOW
#define OFF	HIGH

const static int  LED_SELECT[] = {LED_R, LED_G, LED_B};




/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static int temperaturas_actuales[4];+

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void Led_Init(void)
{
	if (!initialized){
		gpioMode (LED_R, OUTPUT, DSE_DEFAULT, ODE_DEFAULT, SRE_DEFAULT, PFE_DEFAULT, DFE_DEFAULT, 0);
		gpioMode (LED_B, OUTPUT, DSE_DEFAULT, ODE_DEFAULT, SRE_DEFAULT, PFE_DEFAULT, DFE_DEFAULT, 0);
		gpioMode (LED_G, OUTPUT, DSE_DEFAULT, ODE_DEFAULT, SRE_DEFAULT, PFE_DEFAULT, DFE_DEFAULT, 0);

		gpioWrite(LED_R, OFF);
		gpioWrite(LED_B, OFF);
		gpioWrite(LED_G, OFF);

		initialized = true;
	}
}

void Led_On(LEDS led)
{
	if (initialized){
		if(led != WHITE_){gpioWrite(LED_SELECT[led], ON);}
		else{
			gpioWrite(LED_R, ON);
			gpioWrite(LED_G, ON);
			gpioWrite(LED_B, ON);
		}
	}
}

void Led_Off(LEDS led)
{
	if (initialized){
		if(led != WHITE_){gpioWrite(LED_SELECT[led], OFF);}
		else{
			gpioWrite(LED_R, OFF);
			gpioWrite(LED_G, OFF);
			gpioWrite(LED_B, OFF);
		}
	}
}

void Led_Toggle(LEDS led)
{
	if (initialized){
		if(led != WHITE_){gpioToggle(LED_SELECT[led]);}
		else{
			gpioToggle(LED_R);
			gpioToggle(LED_G);
			gpioToggle(LED_B);
		}
	}
}

void Led_Color(LED_COLOR color){
	if (initialized){
		gpioWrite(LED_R, ! (color & (1<<0)));
		gpioWrite(LED_G, ! (color & (1<<1)));
		gpioWrite(LED_B, ! (color & (1<<2)));
	}
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
