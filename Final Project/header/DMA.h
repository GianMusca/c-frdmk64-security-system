#include "gpio.h"

typedef enum{
	DMA_ch0,
	DMA_ch1,
	DMA_ch2,
	DMA_ch3,
	DMA_ch4,
	DMA_ch5,
	DMA_ch6,
	DMA_ch7,
	DMA_ch8,
	DMA_ch9,
	DMA_ch10,
	DMA_ch11,
	DMA_ch12,
	DMA_ch13,
	DMA_ch14,
	DMA_ch15
} DMA_channel;

typedef enum{
	DMA_8bit,
	DMA_16bit,
	DMA_32bit,
	//RESERVED
	DMA_16byte = 0x4u,
	DMA_32byte
	//RESERVED
	//RESERVED
} DMA_transferSize;

void DMA_Init(void);
void DMA_Request_Config(DMA_channel channel, pin_t pin, uint8_t IRQMode);
void DMA_Config(DMA_channel channel,uint32_t saddr,uint32_t daddr,
		uint32_t soff,uint32_t doff,
		DMA_transferSize tSize, uint32_t mlno, uint32_t citer,
		uint32_t slast, uint32_t dlast);
void DMA_Set_IRQ_Handler(void(*callback)(void));
void DMA_quick_enable_IRQ(pin_t pin, uint8_t IRQMode);
void DMA_quick_disable_IRQ(pin_t pin);
void DMA_quick_config(DMA_channel channel,uint32_t saddr,uint32_t daddr,
		uint32_t soff,uint32_t doff,
		DMA_transferSize tSize, uint32_t mlno, uint32_t citer,
		uint32_t slast, uint32_t dlast);

uint32_t DMA_get_saddr(DMA_channel channel);
uint32_t DMA_get_daddr(DMA_channel channel);
