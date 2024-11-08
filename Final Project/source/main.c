#include <jpec.h>
#include <my_semaphores.h>
#include "hardware.h"
#include  <os.h>
#include "timer.h"

#include "Led_DRV.h"

#include "bitmap_matrix.h"
#include "OV7670.h"

#include <string.h>
#include <stdbool.h>
#include "stdint.h"

#include "base64.h"
#include "magnetic_app.h"
#include "id_identification.h"

//

#include "lwip/opt.h"

//#include <string.h> /* strlen, memcpy */

//#if LWIP_IPV4 && LWIP_DHCP

#include "lwip/timeouts.h"
#include "lwip/ip_addr.h"
#include "lwip/init.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "netif/ethernet.h"
#include "enet_ethernetif.h"
#include "lwip/apps/smtp.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "board2.h"
#include "fsl_phy.h"

#include "fsl_phyksz8081.h"
#include "fsl_enet_mdio.h"
#include "fsl_device_registers.h"

/* HTTP Server */
#include "lwip/apps/httpd.h"

/* SNTP */
#include "lwip/apps/sntp.h"
#include "lwip/netif.h"
#include <time.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* @TEST_ANCHOR */

/* MAC address configuration. */
#ifndef configMAC_ADDR
#define configMAC_ADDR                     \
    {                                      \
        0x02, 0x12, 0x13, 0x10, 0x15, 0x11 \
    }
#endif
/* Address of PHY interface. */
#define EXAMPLE_PHY_ADDRESS BOARD_ENET0_PHY_ADDRESS

/* MDIO operations. */
#define EXAMPLE_MDIO_OPS enet_ops

/* PHY operations. */
#define EXAMPLE_PHY_OPS phyksz8081_ops

/* ENET clock frequency. */
#define EXAMPLE_CLOCK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)

/* System clock name. */
#define EXAMPLE_CLOCK_NAME kCLOCK_CoreSysClk

/* CGI */
#define N_CGI_HANDLERS 4

/* SSI */
#define N_SSI_TAGS 14

/*Cards*/
//#define N_CARDS 10

/* Email */
#define N_EMAIL 1

/* Area */
#define N_COORDS 4
#define XMIN 127
#define XMAX 871
#define YMIN 153
#define YMAX 711

/* SMTP Server Config */
#define SERVER_IP "192.168.0.26"
#define SERVER_PORT  25
#define USER_AUTH "loquesea@loquesea.com"
#define PASS_AUTH "loquesea"

#ifndef EXAMPLE_NETIF_INIT_FN
/*! @brief Network interface initialization function. */
#define EXAMPLE_NETIF_INIT_FN ethernetif0_init
#endif /* EXAMPLE_NETIF_INIT_FN */

enum data_states {
	EMAIL_START = 0,
	FIRST_ID,
	AFTER_FIRST_ID,
	ACCESS_TIME,
	AFTER_TIME,
	SECOND_ID,
	BET_FILENAME,
	FILE_TIME,
	AFTER_FILE,
	IMAGE,
	EMAIL_END,
	FINISHED
};




/* Task Start */
#define TASKSTART_STK_SIZE 		1024u
#define TASKSTART_PRIO 			2u
static OS_TCB TaskStartTCB;
static CPU_STK TaskStartStk[TASKSTART_STK_SIZE];

/* Task 2 */
#define TASK2_STK_SIZE			1024u
#define TASK2_STK_SIZE_LIMIT	(TASK2_STK_SIZE / 10u)
#define TASK2_PRIO              3u
static OS_TCB Task2TCB;
static CPU_STK Task2Stk[TASK2_STK_SIZE];

/* Task 3 */
#define TASK3_STK_SIZE			1024u
#define TASK3_STK_SIZE_LIMIT	(TASK3_STK_SIZE / 10u)
#define TASK3_PRIO              4u
static OS_TCB Task3TCB;
static CPU_STK Task3Stk[TASK3_STK_SIZE];


#define MAX_QUEUE 100
#define N_PEND_OBJ 2
#define N_OF_FLOORS 3

//Timers (1000 Hz)
#define TIMEOUT 5000 		//20s
#define ALIVE_RATE	10000	//40s


//State_Type *p2state;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

tim_id_t alive_timer;

static OS_SEM alive_evnt;
OS_SEM server_rta;

static OS_SEM photo_ready_sem;

//static int encoded_jpg_size;

/*******************************************************************************
 * Variables
 ******************************************************************************/

static mdio_handle_t mdioHandle = {.ops = &EXAMPLE_MDIO_OPS};
static phy_handle_t phyHandle   = {.phyAddr = EXAMPLE_PHY_ADDRESS, .mdioHandle = &mdioHandle, .ops = &EXAMPLE_PHY_OPS};
static int enviar_email = 0;


static char* email_from = "fakeemail@fake.com";
static char email_to[50] = "tdemeco@itba.edu.ar";

#define USER_ID_LEN 8
//static char* user_id = "15681239";


#define EMAIL_START_LEN 161
static char* email_data_start = "\r\n"
		"--977d81ff9d852ab2a0cad646f8058349\r\n"
		"Content-Type: text/plain; charset=\"utf-8\"\r\n"
		"Content-Transfer-Encoding: quoted-printable\r\n\r\n"
		"Atención,=0A=0AEl usuario de ID ";

#define AFTER_FIRST_ID_LEN 46
static char* email_data_after_id = " ha accedido al edificio.=0ATiempo de Acceso: ";

#define ACCESS_TIME_LEN 23
//static char* access_time = "19/02/2022 15:03:34:958";

#define AFTER_TIME_LEN 204
static char* email_data_after_time = "=0A=0ASe envía adjunta imagen del usuario:=0A=0A\r\n\r\n"
	"--977d81ff9d852ab2a0cad646f8058349\r\n"
	"Content-Type: text/plain\r\n"
	"Content-Transfer-Encoding: base64\r\n"
	"Content-Disposition: attachment; filename=\"Acceso_ID:_";


#define BET_FILENAME_LEN 8
static char* between_filename = "_Fecha:_";

#define FILE_TIME_LEN 23
//static char* access_time_file = "19-02-2022_15.03.34.958"; //Esto es para adaptarlo a nombre de archivo

#define AFTER_FILE_LEN 9
static char* after_filename = ".jpg\"\r\n\r\n";

#define EMAIL_END_LEN 42
static char* end_email = "\r\n\r\n--977d81ff9d852ab2a0cad646f8058349--\r\n";

/* SNTP */

static struct tm* ptr_current_time_val;
//static struct tm* ptr_current_time_file_val;

static char access_time[32];
static char access_time_file[32];


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void my_port_init(void);

void photo_is_ready(void);
void compress_and_send_photo(const uint8_t* id);

/* Initializers */
void myCGIinit(void);
void mySSIinit(void);

/* CGI Handlers */
static const char* EmailCGIhandler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
static const char* CardsCGIhandler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
static const char* AreaCGIhandler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
static const char* UpdateCGIhandler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);

/* SSI Handler */
static u16_t mySSIHandler(int iIndex, char *pcInsert, int iInsertLen);

/* SNTP */
void sntp_set_system_time(u32_t sec);
void sntp_example_init(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/* CGI Tables */
static const tCGI EmailCGI = { "/email.cgi", EmailCGIhandler };
static const tCGI CardsCGI = { "/cards.cgi", CardsCGIhandler };
static const tCGI AreaCGI = { "/area.cgi", AreaCGIhandler };
static const tCGI UpdateCGI = { "/update.cgi", UpdateCGIhandler };


/* SSI Table */
static char const *theSSItags[N_SSI_TAGS] = { "mail","image"
	,"card01","card02","card03","card04","card05","card06","card07","card08","card09","card10"};

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
/* CGI Table Index */
static tCGI theCGItable[N_CGI_HANDLERS];

//static char Email[50] = " ";
//static char Cards[N_CARDS][ID_SIZE+1] = {"60612683","46605701","60105667","45176601","","","","","",""};
static int Area[N_COORDS] = {0,0,0,0};

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 *******************************************************************************/
static const uint8_t* card_id;

void close_door(void){
	Led_Color(BLACK);
}

static void Task2(void *p_arg){
    (void)p_arg;
    OS_ERR os_err;
    bool id_in_list = false;
    tim_id_t led_timer;

    READING_STATE_MA magnetic_state;


    init_magnetic_ma();
    enable_reading_ma();
    //disable_reading_ma();

    timerInit();

    Led_Color(BLACK);
    led_timer = timerGetId();

    while (1) {
    	OSSemPend(&magnetic_updated_sem,0u,OS_OPT_PEND_BLOCKING,(CPU_TS*)0,&os_err);
    	magnetic_state = get_state_ma();
    	if (magnetic_state == NEW_VALID_DATA_MA){

    		OSMutexPend(&id_mutex,0u,OS_OPT_PEND_BLOCKING,(CPU_TS*)0,&os_err);
    		card_id = get_data_ma();
    		OSMutexPost(&id_mutex,OS_OPT_POST_NONE,&os_err);

    		OSMutexPend(&id_list_mutex,0u,OS_OPT_PEND_BLOCKING,(CPU_TS*)0,&os_err);
    		id_in_list = search_id_in_list(card_id);
    		OSMutexPost(&id_list_mutex,OS_OPT_POST_NONE,&os_err);
    		if(id_in_list){
    			disable_reading_ma();
    			//Led_Color(YELLOW);

    			OSMutexPend(&bitmap_mutex,0u,OS_OPT_PEND_BLOCKING,(CPU_TS*)0,&os_err);
    			OV7670_TakePicture(&photo_is_ready);
    			OSSemPend(&photo_ready_sem,0u,OS_OPT_PEND_BLOCKING,(CPU_TS*)0,&os_err);
    			OSMutexPost(&bitmap_mutex,OS_OPT_POST_NONE,&os_err);

    			compress_and_send_photo(card_id);
    			Led_Color(GREEN);
    			timerStart(led_timer,3000,TIM_MODE_SINGLESHOT,close_door);

    			enable_reading_ma();
    		}else{
    			//Led_Color(RED);
    		}
    	}else if(magnetic_state == WAITING_MA){
    		//Led_Color(CYAN);
    	}else if(magnetic_state == INVALID_DATA_MA){
    		//Led_Color(WHITE);
    	}else{
    		//Led_Color(MAGENTA);
    	}
	}
}

static void print_dhcp_state(struct netif *netif);
void send_email(void);
static void Task3(void *p_arg){
    (void)p_arg;
    OS_ERR os_err;

    struct netif netif;
    ip4_addr_t netif_ipaddr, netif_netmask, netif_gw;
    ethernetif_config_t enet_config = {
        .phyHandle  = &phyHandle,
        .macAddress = configMAC_ADDR,
    };

    SYSMPU_Type *base = SYSMPU;

    /* Disable SYSMPU. */
    base->CESR &= ~SYSMPU_CESR_VLD_MASK;

    mdioHandle.resource.csrClock_Hz = EXAMPLE_CLOCK_FREQ;

    IP4_ADDR(&netif_ipaddr, 0U, 0U, 0U, 0U);
    IP4_ADDR(&netif_netmask, 0U, 0U, 0U, 0U);
    IP4_ADDR(&netif_gw, 0U, 0U, 0U, 0U);

    netif_add(&netif, &netif_ipaddr, &netif_netmask, &netif_gw, &enet_config, EXAMPLE_NETIF_INIT_FN, ethernet_input);
    netif_set_default(&netif);
    netif_set_up(&netif);

    dhcp_start(&netif);

    httpd_init();

    myCGIinit();
    mySSIinit();

    while(1){
        /* Poll the driver, get any outstanding frames */
        ethernetif_input(&netif);
        /* Handle all system timeouts for all core protocols */
        sys_check_timeouts();

        print_dhcp_state(&netif);


        OSSemPend(&send_email_sem,0u,OS_OPT_PEND_NON_BLOCKING,(CPU_TS*)0,&os_err);

        if(os_err == OS_ERR_NONE){
        	send_email();
        	PRINTF("\r\nEnviando Mail\r\n");

        }
    }
}

static void TaskStart(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    /* Initialize the uC/CPU Services. */
    CPU_Init();

#if OS_CFG_STAT_TASK_EN > 0u
    /* (optional) Compute CPU capacity with no task running */
    OSStatTaskCPUUsageInit(&os_err);
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    /* Create semaphore */
    OSSemCreate(&magnetic_updated_sem, "New Input Semaphore",0u,&os_err);
    OSMutexCreate(&bitmap_mutex, "Bitmap Mutex",&os_err);
    OSMutexCreate(&id_mutex, "ID Mutex",&os_err);
    OSSemCreate(&send_email_sem, "Send Email Semaphore",0u,&os_err);
    OSMutexCreate(&email_mutex, "Email Mutex",&os_err);
    OSMutexCreate(&id_list_mutex, "ID List Mutex",&os_err);

//    OSQCreate(&queue, "Message Queue", MAX_QUEUE,&os_err);

    OSSemCreate(&alive_evnt, "Sem Alive", 0u, &os_err);
    OSSemCreate(&server_rta, "Sem Server Rta", 0u, &os_err);

    OSSemCreate(&photo_ready_sem, "Photo Rready", 0u, &os_err);

    timerInit();
    Led_Init();

    OV7670_Init();
    /* Create Task2 */

    OSTaskCreate(&Task2TCB, 			//tcb
                 "Task 2",				//name
                  Task2,				//func
                  0u,					//arg
                  TASK2_PRIO,			//prio
                 &Task2Stk[0u],			//stack
                  TASK2_STK_SIZE_LIMIT,	//stack limit
                  TASK2_STK_SIZE,		//stack size
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);

    OSTaskCreate(&Task3TCB, 			//tcb
                 "Task 3",				//name
                  Task3,				//func
                  0u,					//arg
                  TASK3_PRIO,			//prio
                 &Task3Stk[0u],			//stack
                  TASK3_STK_SIZE_LIMIT,	//stack limit
                  TASK3_STK_SIZE,		//stack size
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);

	//sem_are_ready = true;

    while (1) {

    	OV7670_run();

    	//SHOULD NEVER GET HERE (?)
    	while(1){
    	}

    }
}

int main(void) {
    OS_ERR err;

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR  cpu_err;
#endif

    hw_Init();
    hw_DisableInterrupts();
    my_port_init();
	//thingspeak_init(serverRtaCallback);
	hw_EnableInterrupts();

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    time_init();
    lwip_init();


    OSInit(&err);
 #if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
	 /* Enable task round robin. */
	 OSSchedRoundRobinCfg((CPU_BOOLEAN)1, 0, &err);
 #endif
    OS_CPU_SysTickInit(SystemCoreClock / (uint32_t)OSCfg_TickRate_Hz);

    OSTaskCreate(&TaskStartTCB,
                 "App Task Start",
                  TaskStart,
                  0u,
                  TASKSTART_PRIO,
                 &TaskStartStk[0u],
                 (TASKSTART_STK_SIZE / 10u),
                  TASKSTART_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 &err);

    OSStart(&err);

	/* Should Never Get Here */
    while (1) {

    }
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/**************
      CGI
 *************/

void myCGIinit(void)
{
    theCGItable[0] = AreaCGI; //Add Area control CGI to the table
    theCGItable[1] = EmailCGI; //Add Email control CGI to the table
    theCGItable[2] = CardsCGI; //Add Cards control CGI to the table
    theCGItable[3] = UpdateCGI; //Add Cards control CGI to the table


    http_set_cgi_handlers(theCGItable, N_CGI_HANDLERS);  //Give the table to the HTTP server
}


static const char* EmailCGIhandler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	OS_ERR os_err;
	if(iIndex == 1)
	{
		OSMutexPend(&email_mutex,0u,OS_OPT_PEND_BLOCKING,(CPU_TS*)0,&os_err);
		int j =0;
		for(int i =0; i <= strlen(pcValue[0]);i++){
			if(pcValue[0][i] == '%')
			{
				email_to[i]='@';
				i++;i++;
				j++;
			}

			else
			{
				email_to[j]=pcValue[0][i];
				j++;
			}
		}
		OSMutexPost(&email_mutex,OS_OPT_POST_NONE,&os_err);
		PRINTF("The save email is: %s \n", email_to);
	}


    // the extension .shtml for SSI to work
    return "/index.shtml";
}



static const char* CardsCGIhandler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	OS_ERR os_err;
	uint32_t i = 0;

	OSMutexPend(&id_list_mutex,0u,OS_OPT_PEND_BLOCKING,(CPU_TS*)0,&os_err);
	for (i = 0; i < iNumParams; i++) {
    	//PRINTF("%s =  %s \n",pcParam[i], pcValue[i]);
    	strcpy(Cards[i], pcValue[i]);

	}
	OSMutexPost(&id_list_mutex,OS_OPT_POST_NONE,&os_err);

    // the extension .shtml for SSI to work
    return "/Web_cards.shtml";
}

static const char* AreaCGIhandler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	uint32_t i = 0;

	for (i = 0; i < 2; i++) {
		int n = atoi(pcValue[i]);
		if(n < XMIN){Area[i] = XMIN;}
		else if(n > XMAX){Area[i] = XMAX;}
		else{Area[i] = n;}
    	PRINTF("%s =  %d \n",pcParam[i], Area[i]);
	}

	for (i = 2; i < iNumParams ; i++) {
		int n = atoi(pcValue[i]);
		if(n < YMIN){Area[i] = YMIN;}
		else if(n > YMAX){Area[i] = YMAX;}
		else{Area[i] = n;}
    	PRINTF("%s =  %d \n",pcParam[i], Area[i]);
	}


    // the extension .shtml for SSI to work
    return "/index.shtml";
}

static const char* UpdateCGIhandler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	PRINTF("\nUpdate camera footage\n");
    // the extension .shtml for SSI to work
	return "/index.shtml";
}


/**************
      SSI
 *************/

void mySSIinit(void)
{
    http_set_ssi_handler(mySSIHandler, (char const**) theSSItags, N_SSI_TAGS);
}


static u16_t mySSIHandler(int iIndex, char *pcInsert, int iInsertLen)
{
    if(iIndex == 0)//Email
    {
    	char a[]="<input value=\"";
		char b[]="\" type=\"email\" class=\"fieldEmail\" placeholder=\"Email...\" name=\"email\">";
    	char myStr3[200];
    	snprintf(myStr3, sizeof myStr3, "%s%s%s", a, email_to, b);

        strcpy(pcInsert, myStr3);
        return strlen(myStr3);
    }

    else if(iIndex == 1)//Image
    {
        char myStr4[] = "<img class=\"center selectDisable\" src=\"Resources/Image.svg\" alt=\"Security Camera\" style=\"width:640px;height:480px\">";
        strcpy(pcInsert, myStr4);
        return strlen(myStr4);
    }

    else if(iIndex >= 2 && iIndex <= 11)//Cards
    {
    	uint8_t i = iIndex -1;

		char a[]="<input value=\"";
		char b[]="\" type=\"text\" class=\"fieldCard\" pattern=\"[0-9]{8}\" placeholder=\"Card 0";
		char c[] = "\" name=\"Card0";


		if(i > 9){
			char aux0[]= "\" type=\"text\" class=\"fieldCard\" pattern=\"[0-9]{8}\" placeholder=\"Card ";
			char aux1[]= "\" name=\"Card" ;
			strcpy(b,aux0);
			strcpy(c,aux1);}

		char d[] = "\">";
    	char myStrx[200];
    	snprintf(myStrx, sizeof myStrx, "%s%s%s%d%s%d%s", a, Cards[i-1], b,i,c,i,d);
        strcpy(pcInsert, myStrx);
        PRINTF("Card:%s \n",Cards[i-1]);
        return strlen(myStrx);
    }



    return 0;

}


/*******************************************************************************
 *******************************************************************************
                        CALLBACK FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void compress_and_send_photo(const uint8_t* id){
	static jpec_enc_t *e = NULL;
	OS_ERR os_err;

	jpg_size = 0;
	jpg_file = NULL;

	if(e != NULL)
		jpec_enc_del(e);

	OSMutexPend(&bitmap_mutex,0u,OS_OPT_PEND_BLOCKING,(CPU_TS*)0,&os_err);
	e = jpec_enc_new((const uint8_t*)bitmap_matrix, 320, 240, calc_buf);

	const uint8_t *jpeg = jpec_enc_run(e, &jpg_size);
	base_64_jpg_size = encode_base64((char*) bitmap_matrix, 320*240, (const char*)jpeg, jpg_size);
	jpg_file = jpeg;

	OSMutexPost(&bitmap_mutex,OS_OPT_POST_NONE,&os_err);

	OSSemPost(&send_email_sem,OS_OPT_POST_ALL,&os_err);

}

void photo_is_ready(void){
	OS_ERR os_err;
	OSSemPost(&photo_ready_sem,OS_OPT_POST_ALL,&os_err);
}


void my_port_init(void){

	/* 	CLOCK GATE	*/
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	/* w1c interrupts */
	PORTA->ISFR = PORT_ISFR_ISF_MASK;
	PORTB->ISFR = PORT_ISFR_ISF_MASK;
	PORTC->ISFR = PORT_ISFR_ISF_MASK;
	PORTD->ISFR = PORT_ISFR_ISF_MASK;
	PORTE->ISFR = PORT_ISFR_ISF_MASK;

	/* Enable interrupts */
	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTB_IRQn);
	NVIC_EnableIRQ(PORTC_IRQn);
	NVIC_EnableIRQ(PORTD_IRQn);
	NVIC_EnableIRQ(PORTE_IRQn);

}


/*******************************************************************************
 * Code
 ******************************************************************************/

int my_smtp_bodydh_fn(void *arg, struct smtp_bodydh *bdh)
{
	if(bdh->state == FINISHED)
		return BDH_DONE;

	switch(bdh->state){
	case EMAIL_START:
		bdh->buffer = email_data_start;
		bdh->length = EMAIL_START_LEN;
		bdh->state = FIRST_ID;
		break;
	case FIRST_ID:
		bdh->buffer = card_id;
		bdh->length = USER_ID_LEN;
		bdh->state = AFTER_FIRST_ID;
		break;
	case AFTER_FIRST_ID:
		bdh->buffer = email_data_after_id;
		bdh->length = AFTER_FIRST_ID_LEN;
		bdh->state = ACCESS_TIME;
		break;
	case ACCESS_TIME:
		bdh->buffer = access_time;
		bdh->length = strlen(access_time);
		bdh->state = AFTER_TIME;
		break;
	case AFTER_TIME:
		bdh->buffer = email_data_after_time;
		bdh->length = AFTER_TIME_LEN;
		bdh->state = SECOND_ID;
		break;
	case SECOND_ID:
		bdh->buffer = card_id;
		bdh->length = USER_ID_LEN;
		bdh->state = BET_FILENAME;
		break;
	case BET_FILENAME:
		bdh->buffer = between_filename;
		bdh->length = BET_FILENAME_LEN;
		bdh->state = FILE_TIME;
		break;
	case FILE_TIME:
		bdh->buffer = access_time_file;
		bdh->length = strlen(access_time_file);
		bdh->state = AFTER_FILE;
		break;
	case AFTER_FILE:
		bdh->buffer = after_filename;
		bdh->length = AFTER_FILE_LEN;
		bdh->state = IMAGE;
		break;
	case IMAGE:
		bdh->buffer = (char*)bitmap_matrix;
		bdh->length = base_64_jpg_size;
		bdh->state = EMAIL_END;
		break;
	case EMAIL_END:
		bdh->buffer = end_email;
		bdh->length = EMAIL_END_LEN;
		bdh->state = FINISHED;
		break;
	default:
		bdh->buffer = NULL;
		bdh->length = 0;
		bdh->state = EMAIL_START;
		break;
	}

    return BDH_WORKING;
}


//this function is called when SMTP server wants to tell us something [- SMTP #2 -]
void mySMTPresult(void *arg, u8_t smtp_result, u16_t srv_err, err_t err) {
	OS_ERR os_err;
//	PRINTF("mail (%p) sent with results: 0x%02x, 0x%04x, 0x%08x\n", arg,
//	smtp_result, srv_err, err);
	/*if (email_body)
		free(email_body);*/

	OSMutexPost(&id_mutex,OS_OPT_POST_NONE,&os_err);
	OSMutexPost(&email_mutex,OS_OPT_POST_NONE,&os_err);
	OSMutexPost(&bitmap_mutex,OS_OPT_POST_NONE,&os_err);

	//Led_Color(GREEN);

} //mySMTPresult

void send_email(void)
{
	OS_ERR os_err;
	OSMutexPend(&bitmap_mutex,0u,OS_OPT_PEND_BLOCKING,(CPU_TS*)0,&os_err);
	OSMutexPend(&email_mutex,0u,OS_OPT_PEND_BLOCKING,(CPU_TS*)0,&os_err);
	OSMutexPend(&id_mutex,0u,OS_OPT_PEND_BLOCKING,(CPU_TS*)0,&os_err);
	smtp_set_server_addr(SERVER_IP);
	smtp_set_server_port(SERVER_PORT);
	smtp_set_auth(USER_AUTH, PASS_AUTH);

/*	int email_body_size = 50000; //Con 50000 nos aseguramos que entre un jpeg de maximo tamaño
	email_body = malloc(email_body_size);
	if (email_body)
	{
		snprintf(email_body, email_body_size, "%s%s%s%s%s%s%s%s%s%s%s", email_data_start,
			user_id, email_data_after_id, access_time, email_data_after_time, user_id, between_filename, access_time_file, after_filename,
			jpeg_base64_image, end_email);
		smtp_send_mail_static(email_from, email_to, user_id, email_body, mySMTPresult, NULL);
	}*/

	smtp_send_mail_bodycback(email_from, email_to, card_id, my_smtp_bodydh_fn, mySMTPresult, NULL);
	//smtp_send_mail_static(email_from, email_to, user_id, email_data, mySMTPresult, NULL);
	enviar_email = 0;
}

static void print_dhcp_state(struct netif *netif)
{
    static u8_t dhcp_last_state = DHCP_STATE_OFF;
    struct dhcp *dhcp           = netif_dhcp_data(netif);

    if (dhcp == NULL)
    {
        dhcp_last_state = DHCP_STATE_OFF;
    }
    else if (dhcp_last_state != dhcp->state)
    {
        dhcp_last_state = dhcp->state;

        PRINTF(" DHCP state       : ");
        switch (dhcp_last_state)
        {
            case DHCP_STATE_OFF:
                PRINTF("OFF");
                break;
            case DHCP_STATE_REQUESTING:
                PRINTF("REQUESTING");
                break;
            case DHCP_STATE_INIT:
                PRINTF("INIT");
                break;
            case DHCP_STATE_REBOOTING:
                PRINTF("REBOOTING");
                break;
            case DHCP_STATE_REBINDING:
                PRINTF("REBINDING");
                break;
            case DHCP_STATE_RENEWING:
                PRINTF("RENEWING");
                break;
            case DHCP_STATE_SELECTING:
                PRINTF("SELECTING");
                break;
            case DHCP_STATE_INFORMING:
                PRINTF("INFORMING");
                break;
            case DHCP_STATE_CHECKING:
                PRINTF("CHECKING");
                break;
            case DHCP_STATE_BOUND:
                PRINTF("BOUND");
                break;
            case DHCP_STATE_BACKING_OFF:
                PRINTF("BACKING_OFF");
                break;
            default:
                PRINTF("%u", dhcp_last_state);
                assert(0);
                break;
        }
        PRINTF("\r\n");

        if (dhcp_last_state == DHCP_STATE_BOUND)
        {
        	sntp_example_init();
            PRINTF("\r\n IPv4 Address     : %s\r\n", ipaddr_ntoa(&netif->ip_addr));
            PRINTF(" IPv4 Subnet mask : %s\r\n", ipaddr_ntoa(&netif->netmask));
            PRINTF(" IPv4 Gateway     : %s\r\n\r\n", ipaddr_ntoa(&netif->gw));
        }
    }
}

/**************
      SNTP
 *************/


void sntp_set_system_time(u32_t sec){
	char buf[32];
	sec -= 3*60*60;
//	struct tm* ptr_current_time_val;
//	struct tm current_time_val;
	time_t current_time = (time_t) sec;

#ifdef LOCALTIME_R
	localtime_r(&current_time, &current_time_val);
	strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", &current_time_val);
#else
	ptr_current_time_val = localtime(&current_time);
	strftime(access_time, sizeof(access_time), "%d.%m.%Y %H:%M:%S", ptr_current_time_val);
	strftime(access_time_file, sizeof(access_time_file), "%d.%m.%Y_%H-%M-%S", ptr_current_time_val);
#endif

	PRINTF(("SNTP time: %s\n", buf));
}

void sntp_example_init(void){
	static bool initializated = false;
	if (!initializated){
		sntp_setoperatingmode(SNTP_OPMODE_POLL);
		sntp_servermode_dhcp(1); /* get SNTP server via DHCP */
		sntp_setservername(0, "pool.ntp.org");
		sntp_init();
		initializated = true;
	}
}
