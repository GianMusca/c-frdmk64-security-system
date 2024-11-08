
#include "FTM.h"
//#include "PORT.h"
#include "GPIO.h"

typedef enum
{
	PORT_eDisabled				= 0x00,
	PORT_eDMARising				= 0x01,
	PORT_eDMAFalling			= 0x02,
	PORT_eDMAEither				= 0x03,
	PORT_eInterruptDisasserted	= 0x08,
	PORT_eInterruptRising		= 0x09,
	PORT_eInterruptFalling		= 0x0A,
	PORT_eInterruptEither		= 0x0B,
	PORT_eInterruptAsserted		= 0x0C,
} PORTEvent_t;


void PWM_Init(void);
void PWM_ISR(void);

uint16_t PWM_modulus = 2-1;
uint16_t PWM_duty    = 1;

/* FTM0 fault, overflow and channels interrupt handler*/
__ISR__ FTM0_IRQHandler(void)
{
	PWM_ISR();
}

void PWM_ISR (void)
{
	FTM_ClearOverflowFlag (FTM0);

	FTM_SetCounter(FTM0, 0, PWM_duty++);  //change DC
	//GPIO_Toggle(PTC, 1 << 8);			  //GPIO pin PTC8
	PWM_duty %= PWM_modulus;
}


void FTM_Init (void)
{
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
	SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
	SIM->SCGC6 |= SIM_SCGC6_FTM2_MASK;
	SIM->SCGC3 |= SIM_SCGC3_FTM2_MASK;
	SIM->SCGC3 |= SIM_SCGC3_FTM3_MASK;

	NVIC_EnableIRQ(FTM0_IRQn);
	NVIC_EnableIRQ(FTM1_IRQn);
	NVIC_EnableIRQ(FTM2_IRQn);
	NVIC_EnableIRQ(FTM3_IRQn);

	FTM0->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	FTM1->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	FTM2->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	FTM3->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;

	/*
	 * TO DO
	 */

	PWM_Init();
}

/// FTM PWM Example

// To Test Connect PC9(IC)-PC8(GPIO)
// or PC9(IC)-PC1(OC)

typedef struct{

	 uint32_t PS		:1;
	 uint32_t PE		:1;
	 uint32_t SRE		:1;
	 uint32_t RESERV0	:1;
	 uint32_t PFE		:1;
	 uint32_t ODE		:1;
	 uint32_t DSE		:1;
	 uint32_t RESERV1	:1;
	 uint32_t MUX		:3;
	 uint32_t RESERV2	:4;
	 uint32_t LK		:1;
	 uint32_t IRQC		:4;
	 uint32_t RESERV3	:4;
	 uint32_t ISF		:1;
	 uint32_t RESERV4	:7;


}FIELD_T;  //PCR Structure

#define GPIO_IN(x) 	(0 << x)
#define GPIO_OUT(x) (1 << x)

#define GPIO__IN 	0x00000000u
#define GPIO__OUT   0xFFFFFFFFu

#define GPIO__LO 	0x00000000u
#define GPIO__HI   	0xFFFFFFFFu


typedef union

{
	FIELD_T    FIELD;
	uint32_t   PCR;

}PCRstr;

void PWM_Init (void)
{

	// PTD 0 as PWM
		PCRstr UserPCR;

		UserPCR.PCR=false;			// Default All false, Set only those needed

		UserPCR.FIELD.DSE=false;
		UserPCR.FIELD.MUX=PORT_mAlt4;
		UserPCR.FIELD.IRQC=PORT_eDisabled;

		//PORT_Configure2 (PORTC,1,UserPCR);
		PORTD->PCR[0]=UserPCR.PCR;

	// PTC 8 as GPIO
		//UserPCR.PCR=false;			// Default All false, Set only those needed

		//UserPCR.FIELD.DSE=true;
		//UserPCR.FIELD.MUX=PORT_mGPIO;
		//UserPCR.FIELD.IRQC=PORT_eDisabled;

		//PORT_Configure2 (PORTC,8,UserPCR);
		//PORTC->PCR[8]=UserPCR.PCR;

		//GPIO_SetDirection(PTC, 8, GPIO__OUT);
		//PTC->PDDR = (PTC->PDDR & ~(1<<8) ) | (GPIO__OUT & (1 << 8));




	FTM_SetPrescaler(FTM3, FTM_PSC_x1);
	FTM_SetModulus(FTM3, PWM_modulus);
	FTM_SetOverflowMode(FTM3, false); // CHECK!!
	FTM_SetWorkingMode(FTM3, 0, FTM_mPulseWidthModulation);			// MSA  / B
	FTM_SetPulseWidthModulationLogic(FTM3, 0, FTM_lAssertedHigh);   // ELSA / B
	FTM_SetCounter(FTM3, 0, PWM_duty);
	FTM_StartClock(FTM3);
	//FTM_StopClock(FTM3);
}




// Setters

void FTM_SetPrescaler (FTM_t ftm, FTM_Prescal_t data)
{
	ftm->SC = (ftm->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(data);
}

void FTM_SetModulus (FTM_t ftm, FTMData_t data)
{
	ftm->CNTIN = 0X00;
	ftm->CNT = 0X00;
	ftm->MOD = FTM_MOD_MOD(data);
}

FTMData_t FTM_GetModulus (FTM_t ftm)
{
	return ftm->MOD & FTM_MOD_MOD_MASK;
}

void FTM_StartClock (FTM_t ftm)
{
	ftm->SC |= FTM_SC_CLKS(0x01);
}

void FTM_StopClock (FTM_t ftm)
{
	ftm->SC &= ~FTM_SC_CLKS(0x01);
}

void FTM_SetOverflowMode (FTM_t ftm, bool mode)
{
	ftm->SC = (ftm->SC & ~FTM_SC_TOIE_MASK) | FTM_SC_TOIE(mode);
}

bool FTM_IsOverflowPending (FTM_t ftm)
{
	return ftm->SC & FTM_SC_TOF_MASK;
}

void FTM_ClearOverflowFlag (FTM_t ftm)
{
	ftm->SC &= ~FTM_SC_TOF_MASK;
}

void FTM_SetWorkingMode (FTM_t ftm, FTMChannel_t channel, FTMMode_t mode)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK)) |
			                      (FTM_CnSC_MSB((mode >> 1) & 0X01) | FTM_CnSC_MSA((mode >> 0) & 0X01));
}

FTMMode_t FTM_GetWorkingMode (FTM_t ftm, FTMChannel_t channel)
{
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK)) >> FTM_CnSC_MSA_SHIFT;
}

void FTM_SetInputCaptureEdge (FTM_t ftm, FTMChannel_t channel, FTMEdge_t edge)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
				                  (FTM_CnSC_ELSB((edge >> 1) & 0X01) | FTM_CnSC_ELSA((edge >> 0) & 0X01));
}

FTMEdge_t FTM_GetInputCaptureEdge (FTM_t ftm, FTMChannel_t channel)
{
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}

void FTM_SetOutputCompareEffect (FTM_t ftm, FTMChannel_t channel, FTMEffect_t effect)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
				                  (FTM_CnSC_ELSB((effect >> 1) & 0X01) | FTM_CnSC_ELSA((effect >> 0) & 0X01));
}

FTMEffect_t FTM_GetOutputCompareEffect (FTM_t ftm, FTMChannel_t channel)
{
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}

void FTM_SetPulseWidthModulationLogic (FTM_t ftm, FTMChannel_t channel, FTMLogic_t logic)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
				                  (FTM_CnSC_ELSB((logic >> 1) & 0X01) | FTM_CnSC_ELSA((logic >> 0) & 0X01));
}

FTMLogic_t FTM_GetPulseWidthModulationLogic (FTM_t ftm, FTMChannel_t channel)
{
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}

void FTM_SetCounter (FTM_t ftm, FTMChannel_t channel, FTMData_t data)
{
	ftm->CONTROLS[channel].CnV = FTM_CnV_VAL(data);
}

FTMData_t FTM_GetCounter (FTM_t ftm, FTMChannel_t channel)
{
	return ftm->CONTROLS[channel].CnV & FTM_CnV_VAL_MASK;
}

void FTM_SetInterruptMode (FTM_t ftm, FTMChannel_t channel, bool mode)
{
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~FTM_CnSC_CHIE_MASK) | FTM_CnSC_CHIE(mode);
}

bool FTM_IsInterruptPending (FTM_t ftm, FTMChannel_t channel)
{
	return ftm->CONTROLS[channel].CnSC & FTM_CnSC_CHF_MASK;
}

void FTM_ClearInterruptFlag (FTM_t ftm, FTMChannel_t channel)
{
	ftm->CONTROLS[channel].CnSC &= ~FTM_CnSC_CHF_MASK;
}

