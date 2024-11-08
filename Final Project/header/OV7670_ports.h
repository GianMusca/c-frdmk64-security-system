#include "gpio.h"

//#define EXTRA_PORT_DEBUG

#define	OV6760_D0	PORTNUM2PIN(PC,0)
#define	OV6760_D1	PORTNUM2PIN(PC,1)
#define	OV6760_D2	PORTNUM2PIN(PC,2)
#define	OV6760_D3	PORTNUM2PIN(PC,3)
#define	OV6760_D4	PORTNUM2PIN(PC,4)
#define	OV6760_D5	PORTNUM2PIN(PC,5)
#define	OV6760_D6	PORTNUM2PIN(PC,7)
#define	OV6760_D7	PORTNUM2PIN(PC,8)

//#define OV6760_SCL	PORTNUM2PIN(PE,24)
//#define OV6760_SDA	PORTNUM2PIN(PE,25)

#define OV6760_VS	PORTNUM2PIN(PD,1)
#define OV6760_HS	PORTNUM2PIN(PD,3)
#define OV6760_PLK	PORTNUM2PIN(PD,2)

//#define OV6760_XLK	PORTNUM2PIN(PD,0)

#ifdef EXTRA_PORT_DEBUG
#define BUTTON_SW2	PORTNUM2PIN(PC,6)
#define BUTTON_SW3	PORTNUM2PIN(PA,4)

#define PB18	PORTNUM2PIN(PB,18)
#define PC16	PORTNUM2PIN(PC,16)
#define PC17	PORTNUM2PIN(PC,17)
#define PB9		PORTNUM2PIN(PB,9)

#endif
