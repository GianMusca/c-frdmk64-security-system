#include "DMA.h"
#include "MK64F12.h"

#include "board.h"
#include "hardware.h"

#include <stdio.h>

// TODO: BORRAR LUEGO:
#include "gpio.h"
#include "OV7670_ports.h"
//

static void(*dma_irqhandler)(void) = NULL;

void DMA_Init(void){
	/* Enable the clock for the eDMA and the DMAMUX. */
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
}

void DMA_Request_Config(DMA_channel channel, pin_t pin, uint8_t IRQMode){
	/* Enable the eDMA channel 0 and set the PORTC as the DMA request source. */
	DMAMUX->CHCFG[channel] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(49 + PIN2PORT(pin));

	gpioMode(pin,INPUT,DSE_DEFAULT,ODE_DEFAULT,SRE_DEFAULT,PFE_DEFAULT,DFE_DEFAULT,0);
	DMA_quick_enable_IRQ(pin,IRQMode); //gpioDMA(pin,IRQMode);//PORTD->PCR[2]|=PORT_PCR_IRQC(0x01);
}

void DMA_quick_enable_IRQ(pin_t pin, uint8_t IRQMode){
	gpioDMA(pin,IRQMode);
}
void DMA_quick_disable_IRQ(pin_t pin){
	gpioDMA(pin,0x00);
}

void DMA_Config(DMA_channel channel,uint32_t saddr,uint32_t daddr,
		uint32_t soff,uint32_t doff,
		DMA_transferSize tSize, uint32_t mlno, uint32_t citer,
		uint32_t slast, uint32_t dlast){

	/* Enable the interrupts for the channel 0. */
	/* Clear all the pending events. */
	NVIC_ClearPendingIRQ(DMA0_IRQn + channel);
	/* Enable the DMA interrupts. */
	NVIC_EnableIRQ(DMA0_IRQn + channel);
	/// ============= INIT TCD0 ===================//
	/* Set memory address for source and destination. */
	DMA_quick_config(channel,saddr,daddr,soff,doff,tSize,mlno,citer,slast,dlast);

	/* Setup control and status register. */

	DMA0->TCD[channel].CSR = DMA_CSR_INTMAJOR_MASK;	//Enable Major Interrupt.

	/* Enable request signal for channel 0. */
	DMA0->ERQ = (uint32_t)(((uint32_t)1)<<channel);//DMA_ERQ_ERQ0_MASK;
}

void DMA_quick_config(DMA_channel channel,uint32_t saddr,uint32_t daddr,
		uint32_t soff,uint32_t doff,
		DMA_transferSize tSize, uint32_t mlno, uint32_t citer,
		uint32_t slast, uint32_t dlast){

	DMA0->TCD[channel].SADDR = saddr; //(uint32_t)(&(PTC->PDIR));
	DMA0->TCD[channel].DADDR = daddr; //(uint32_t)(aux_buffer);

	/* Set an offset for source and destination address. */
	DMA0->TCD[channel].SOFF = soff;//=0x00; // Source address offset of 2 bytes per transaction.
	DMA0->TCD[channel].DOFF = doff;//=0x02; // Destination address offset of 1 byte per transaction.

	/* Set source and destination data transfer size is 1 byte. */
	DMA0->TCD[channel].ATTR = DMA_ATTR_SSIZE(tSize) | DMA_ATTR_DSIZE(tSize); //DMA_ATTR_SSIZE(1) | DMA_ATTR_DSIZE(1);

	/*Number of bytes to be transfered in each service request of the channel.*/
	DMA0->TCD[channel].NBYTES_MLNO= mlno;//0x02;

	/* Current major iteration count (5 iteration of 1 byte each one). */
	DMA0->TCD[channel].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(citer);//DMA_CITER_ELINKNO_CITER(320*4);
	DMA0->TCD[channel].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(citer);//DMA_BITER_ELINKNO_BITER(320*4);

	DMA0->TCD[channel].SLAST = slast;//0;
	DMA0->TCD[channel].DLAST_SGA = dlast;//-2*4*320;
}

uint32_t DMA_get_saddr(DMA_channel channel){
	return DMA0->TCD[channel].SADDR;
}
uint32_t DMA_get_daddr(DMA_channel channel){
	return DMA0->TCD[channel].DADDR;
}

void DMA_Set_IRQ_Handler(void(*callback)(void)){
	dma_irqhandler = callback;
}

/*********************************************************/

/* The blue LED is toggled when a TCD is completed. */
__ISR__ DMA0_IRQHandler(void)
{
#ifdef EXTRA_PORT_DEBUG
	gpioWrite(PC17,true);
#endif

	DMA0->CINT |= 0;

	if(dma_irqhandler != NULL)
		dma_irqhandler();

#ifdef EXTRA_PORT_DEBUG
	gpioWrite(PC17,false);
#endif
}

/* The red LED is toggled when an error occurs. */
__ISR__ DMA_Error_IRQHandler(void)
{
	/* Clear the error interrupt flag.*/
	DMA0->CERR |= 0;

	/* Turn the red LED on. */
	gpioToggle(PIN_LED_RED);

}


