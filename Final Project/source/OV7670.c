#include "OV7670.h"
#include "OV7670_ports.h"
#include "gpio.h"
#include "I2C_FIFO_driver.h"
#include "FTM.h"
#include <stdio.h>

#include "bitmap_matrix.h"

#include "board.h"
#include "DMA.h"

#include "os.h"

static OV7670_state ov7670_state = OV7670_not_initialized;
OS_SEM semProcessNewLine;

static bool ov7670_picture_requested = false;
static int ov7670_current_row = 0;
static bool ov7670_active_row = true;
static bool ov7670_row_complete = false;

static OV7670_callback ov7670_picture_ready_callback= NULL;

static OS_ERR os_err;

#define DEBUG_NO_TEST_OUTPUT			0
#define DEBUG_SHIFTING_1				1
#define DEBUG_8_BAR_COLOR_BAR			2
#define DEBUG_FADE_TO_GRAY_COLOR_BAR	3
#define DEBUG_TEST_SELECT				DEBUG_NO_TEST_OUTPUT

#define DEBUG_LOOP_CONFIG	0

#if   DEBUG_TEST_SELECT == DEBUG_NO_TEST_OUTPUT
#define Ox70MASK 0x00U
#define Ox71MASK 0x00U
#elif DEBUG_TEST_SELECT == DEBUG_SHIFTING_1
#define Ox70MASK 0x00U
#define Ox71MASK 0x80U
#elif DEBUG_TEST_SELECT == DEBUG_8_BAR_COLOR_BAR
#define Ox70MASK 0x80U
#define Ox71MASK 0x00U
#elif DEBUG_TEST_SELECT == DEBUG_FADE_TO_GRAY_COLOR_BAR
#define Ox70MASK 0x80U
#define Ox71MASK 0x80U
#endif



static uint8_t OV7670_init[][2] = {
		//{0x11U,0x82U}, // PRESCALER


		{0x11U,0x0FU}, //PRESCALER 0x02U . Por alguna razon, 0x00 = /2
		{0x15U,0x20U}, // NO PCLK MIENTRAS HORIZONTAL ... COSO
		//{0x70U,0x3AU | Ox70MASK}, // TEST PATTERN
		//{0x71U,0x35U | Ox71MASK} //, // TEST PATTERN


/*
		{0x07U,0x00U}, //AEC 3F
		{0x10U,0x20U}, //AEC FF
		{0x04U,0x00U}, //AEC 03
		{0x3BU,0x8AU} //, //NIGHT MODE
*/
/*
		{0x13U,0x8FU | (1<<5)}, //BANDING FILTER ON
		{0x1EU,0x01U | (0<<2)}, //BLACK SUN ENABLE
		{0x3AU,0x05U | (1<<3)}, // Output sequence (1-)
		{0x3DU,0x88U | (0<<0)}//, // Output sequence (-0) UYVY

		*/
		/*
		{0x14U,0x0AU | 0x40}, //MAXIMUM AGC VALUE (automatic gain ceiling)
		{0x24U,0x75U}, //AGC/AEC stable operation region (upper limit) 0x75U
		{0x25U,0x63U}, //AGC/AEC stable operation region (lower limit) 0x63U
		{0x26U,0xD4U}//, // AGC/AEC Fast mode op.region 0xD4
		*/

		//{0x12U,0x00U}, //YUV... que se yo

		{0x32U,0xF6U}, //HREF
		{0x17U,0x13U}, //HSTART
		{0x18U,0x01U}, //HSTOP

		/*
		{0x19U,0x02U}, //VSTART
		{0x1AU,0x7AU}, //VSTOP
		{0x03U,0x0AU}  //VREF
*/

};

static uint16_t aux_buffer[320*4];

void DMA_mayor_int(void);
void vertical_sync_rising(void);
void horizontal_ref_falling(void);
void I2C_finished_callback(void);
void OV7670_process_new_line(void);

void OV7670_Init(void){
	if(ov7670_state == OV7670_not_initialized){
#ifdef EXTRA_PORT_DEBUG
		gpioToggle(PIN_LED_BLUE);
#endif
		OSSemCreate(&semProcessNewLine, "Process New Line Semaphore",0u,&os_err);

		ov7670_picture_requested = false;
		ov7670_picture_ready_callback = NULL;

		gpioMode(OV6760_D0,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioMode(OV6760_D1,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioMode(OV6760_D2,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioMode(OV6760_D3,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioMode(OV6760_D4,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioMode(OV6760_D5,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioMode(OV6760_D6,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioMode(OV6760_D7,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);

		gpioMode(OV6760_VS,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioIRQ(OV6760_VS,GPIO_IRQ_MODE_RISING_EDGE,vertical_sync_rising);
		gpioMode(OV6760_HS,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioIRQ(OV6760_HS,GPIO_IRQ_MODE_FALLING_EDGE,horizontal_ref_falling);

		//gpioMode(OV6760_PLK,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		//PORTD->PCR[2]|=PORT_PCR_IRQC(0x01);

#ifdef EXTRA_PORT_DEBUG
		gpioMode(PB18,OUTPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioWrite(PB18,false);
		gpioMode(PC16,OUTPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioWrite(PC16,false);
		gpioMode(PC17,OUTPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioWrite(PC17,false);
		gpioMode(PB9,OUTPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
		gpioWrite(PB9,false);
#endif
		FTM_Init();
		//I2C_Init();
		//I2C_End_Routine(I2C_finished_callback);
		I2C_FIFO_init();
		I2C_FIFO_empty_callback(I2C_finished_callback);

#if DEBUG_LOOP_CONFIG == 0
		for(int i=0;i<sizeof(OV7670_init)/sizeof(OV7670_init[0]);i++){
			OV7670_Config(OV7670_init[i]);
			//for(int j=0;j<1000000;j++){}
		}
#else
		while(1){
			static int i = 0;
			OV7670_Config(OV7670_init[i]);
			i++;
			if(i>=sizeof(OV7670_init)/sizeof(OV7670_init[0]))
				i=0;
			for(int j=0;j<1000000;j++){}
		}
#endif

		DMA_Init();
		DMA_Config(DMA_ch0, (uint32_t)(&(PTC->PDIR)), (uint32_t)(aux_buffer),
				0x00,0x02,
				DMA_16bit,0x02,320*4,
				0,-2*4*320);
		DMA_Set_IRQ_Handler(DMA_mayor_int);

		DMA_Request_Config(DMA_ch0,OV6760_PLK,0x00); //NOTA: disabled

#ifdef EXTRA_PORT_DEBUG
		gpioToggle(PIN_LED_BLUE);
#endif
	}
}
void OV7670_Config(uint8_t* register_and_data){
	ov7670_state = OV7670_is_busy;
	//gpioWrite(PIN_LED_GREEN,1);
	I2C_FIFO_new_message(false,0x0021,register_and_data,NULL,2,0);
}

void I2C_finished_callback(void){
	ov7670_state = OV7670_is_ready;

	//gpioWrite(PIN_LED_GREEN,0);
}

OV7670_state OV7670_GetState(void){
	return ov7670_state;
}
void OV7670_TakePicture(OV7670_callback callback){
	ov7670_picture_ready_callback = callback;
	ov7670_picture_requested = true;
}

bool OV7670_GetPicture(char** picture){
	*picture = (char*)bitmap_matrix;
	return (ov7670_state == OV7670_new_picture);
}


void vertical_sync_rising(void){

#ifdef EXTRA_PORT_DEBUG
	gpioWrite(PB9,true);
#endif

	if(ov7670_state != OV7670_error){
		if(ov7670_state == OV7670_is_taking_picture){ //FINALIZAR FOTO
			DMA_quick_disable_IRQ(OV6760_PLK);
			ov7670_state = OV7670_is_busy;
			// TODO: PROCESS FOTO OR SOMETHING IDK
			ov7670_state = OV7670_new_picture;
			if(ov7670_picture_ready_callback != NULL)
				ov7670_picture_ready_callback();

		}else if(ov7670_picture_requested && (ov7670_state == OV7670_is_ready || ov7670_state == OV7670_new_picture))
			{		//PREPARARSE PARA NUEVA FOTO

			ov7670_picture_requested = false;

			ov7670_current_row = 0;
			ov7670_active_row = true;
			ov7670_row_complete = false;

			DMA_quick_config(DMA_ch0, (uint32_t)(&(PTC->PDIR)), (uint32_t)(aux_buffer),
					0x00,0x02,
					DMA_16bit,0x02,320*4,
					0,-2*4*320);

			ov7670_state = OV7670_is_taking_picture;
			DMA_quick_enable_IRQ(OV6760_PLK,0x01);
		}
	}

#ifdef EXTRA_PORT_DEBUG
	gpioWrite(PB9,false);
#endif
}


void DMA_mayor_int(void){
	if(ov7670_state != OV7670_error){
		if(ov7670_row_complete){
			ov7670_state = OV7670_error;
#ifdef EXTRA_PORT_DEBUG
			while(1){}
#endif
		}else{
			ov7670_row_complete = true;
		}
	}
}


void horizontal_ref_falling(void){
//	//si esta habilitado la lectura, los datos del arreglo auxiliar los traduce
//	static OS_ERR os_err;
//	gpioWrite(PORTNUM2PIN(PC,16),true);
//
//	if(sem_are_ready)
//		OSSemPost(&semProcessNewLine,OS_OPT_POST_ALL,&os_err);
//
//	gpioWrite(PORTNUM2PIN(PC,16),false);
#ifdef EXTRA_PORT_DEBUG
	//gpioWrite(PC16,true);
	gpioToggle(PC16);
#endif
	if(ov7670_state != OV7670_error && ov7670_state == OV7670_is_taking_picture){
		if(ov7670_active_row){
			DMA_quick_disable_IRQ(OV6760_PLK);
			ov7670_active_row = false;

			if(ov7670_current_row>=240){
				ov7670_state = OV7670_error;
				#ifdef EXTRA_PORT_DEBUG
				while(1){}
				#endif
			}else if(!ov7670_row_complete){
				ov7670_state = OV7670_error;
				#ifdef EXTRA_PORT_DEBUG
				while(1){}
				#endif
			}else if(DMA_get_daddr(DMA_ch0) != (uint32_t)(aux_buffer)){
				ov7670_state = OV7670_error;
				#ifdef EXTRA_PORT_DEBUG
				while(1){}
				#endif
			}else{
				ov7670_row_complete = false;
				OSSemPost(&semProcessNewLine,OS_OPT_POST_ALL,&os_err);
			}

		}else{
			ov7670_current_row++;
			ov7670_active_row = true;
			DMA_quick_enable_IRQ(OV6760_PLK,0x01);
		}
	}
#ifdef EXTRA_PORT_DEBUG
	//gpioWrite(PC16,false);
	gpioToggle(PC16);
	gpioToggle(PC16);
#endif
}

void OV7670_run(void){
	while(1){
		OSSemPend(&semProcessNewLine,0u,OS_OPT_PEND_BLOCKING,(CPU_TS*)0,&os_err);
		OV7670_process_new_line();
	}
}

void OV7670_process_new_line(void){
#ifdef EXTRA_PORT_DEBUG
	gpioWrite(PORTNUM2PIN(PB,18),true);
#endif

	for(int col = 0; col<320; col++){
		uint16_t aux = aux_buffer[(col<<2) + 1];
		bitmap_matrix[ov7670_current_row][col] = ((aux & 0x003fu) | ((aux & 0x0180u)>>1));// & 0x00ff;
	}


#ifdef EXTRA_PORT_DEBUG
	gpioWrite(PORTNUM2PIN(PB,18),false);
#endif
}
