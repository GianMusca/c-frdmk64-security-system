/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "lwip/opt.h"

#include <string.h> /* strlen, memcpy */

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

/* GPIO pin configuration. */
#define BOARD_LED_GPIO       BOARD_LED_RED_GPIO
#define BOARD_LED_GPIO_PIN   BOARD_LED_RED_GPIO_PIN
#define BOARD_SW_GPIO        BOARD_SW3_GPIO
#define BOARD_SW_GPIO_PIN    BOARD_SW3_GPIO_PIN
#define BOARD_SW_PORT        BOARD_SW3_PORT
#define BOARD_SW_IRQ         BOARD_SW3_IRQ
#define BOARD_SW_IRQ_HANDLER BOARD_SW3_IRQ_HANDLER

/* SMTP Server Config */
#define SERVER_IP "192.168.0.120"
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

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void *array_concat(const void *a, size_t an, const void *b, size_t bn);
/*******************************************************************************
 * Variables
 ******************************************************************************/

static mdio_handle_t mdioHandle = {.ops = &EXAMPLE_MDIO_OPS};
static phy_handle_t phyHandle   = {.phyAddr = EXAMPLE_PHY_ADDRESS, .mdioHandle = &mdioHandle, .ops = &EXAMPLE_PHY_OPS};
static int enviar_email = 0;


static char* email_from = "fakeemail@fake.com";
static char* email_to = "demecotobias@gmail.com";

#define USER_ID_LEN 8
static char* user_id = "15681239";


#define EMAIL_START_LEN 161
static char* email_data_start = "\r\n"
		"--977d81ff9d852ab2a0cad646f8058349\r\n"
		"Content-Type: text/plain; charset=\"utf-8\"\r\n"
		"Content-Transfer-Encoding: quoted-printable\r\n\r\n"
		"Atención,=0A=0AEl usuario de ID ";

#define AFTER_FIRST_ID_LEN 46
static char* email_data_after_id = " ha accedido al edificio.=0ATiempo de Acceso: ";

#define ACCESS_TIME_LEN 23
static char* access_time = "19/02/2022 15:03:34:958";

#define AFTER_TIME_LEN 204
static char* email_data_after_time = "=0A=0ASe envía adjunta imagen del usuario:=0A=0A\r\n\r\n"
	"--977d81ff9d852ab2a0cad646f8058349\r\n"
	"Content-Type: text/plain\r\n"
	"Content-Transfer-Encoding: base64\r\n"
	"Content-Disposition: attachment; filename=\"Acceso_ID:_";


#define BET_FILENAME_LEN 8
static char* between_filename = "_Fecha:_";

#define FILE_TIME_LEN 23
static char* access_time_file = "19-02-2022_15.03.34.958"; //Esto es para adaptarlo a nombre de archivo

#define AFTER_FILE_LEN 9
static char* after_filename = ".jpg\"\r\n\r\n";

static char* jpeg_base64_image = "/9j/4QC6RXhpZgAATU0AKgAAAAgACAESAAMAAAABAAEAAAEaAAUAA"
		"AABAAAAbgEbAAUAAAABAAAAdgEoAAMAAAABAAIAAAExAAIAAAAQAAAAfgEyAAIAAAAUAAAAjgE8A"
		"AIAAAAQAAAAogITAAMAAAABAAEAAAAAAAAASAAAAAEAAABIAAAAAQAAUXVpY2tUaW1lIDcuMS4zA"
		"DIwMDY6MDk6MjQgMTI6NTg6MzUATWFjIE9TIFggMTAuNC43AP/+AAxBcHBsZU1hcmsK/9sAhAABA"
		"QEBAQEBAQEBAgIBAgIDAgICAgIDAwMCAwQEBAQEBAQEBAUGBQQFBgUEBAUHBQYGBgcHBwQFBwgHB"
		"wgGBwcHAQICAgICAgMCAgMHBQQFBwcHBwcHBwcHBwcHBwcHBwcHBwcHBwcHBwcHBwcHBwcHBwcHB"
		"wcHBwcHBwcHBwcHBwf/xAGiAAABBQEBAQEBAQAAAAAAAAAAAQIDBAUGBwgJCgsBAAMBAQEBAQEBA"
		"QEAAAAAAAABAgMEBQYHCAkKCxAAAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBk"
		"aEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZna"
		"Glqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1"
		"NXW19jZ2uHi4+Tl5ufo6erx8vP09fb3+Pn6EQACAQIEBAMEBwUEBAABAncAAQIDEQQFITEGEkFRB"
		"2FxEyIygQgUQpGhscEJIzNS8BVictEKFiQ04SXxFxgZGiYnKCkqNTY3ODk6Q0RFRkdISUpTVFVWV"
		"1hZWmNkZWZnaGlqc3R1dnd4eXqCg4SFhoeIiYqSk5SVlpeYmZqio6Slpqeoqaqys7S1tre4ubrCw"
		"8TFxsfIycrS09TV1tfY2dri4+Tl5ufo6ery8/T19vf4+fr/wAARCADwAUADASEAAhEBAxEB/9oAD"
		"AMBAAIRAxEAPwD++T+ztP8A+fCH/v0v+FJ/Z2n/APPhD/36X/CqvLuTp2F/s7T/APnwh/79L/hR/"
		"Z2n/wDPhD/36X/CjXuGnYP7O0//AJ8If+/S/wCFH9naf/z4Q/8Afpf8KNe4WXYT+zdO/wCfCH/v2"
		"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"
		"ZjUwPamVF2ZKhwcdqeRTT1NGf/Z";

#define EMAIL_END_LEN 42
static char* end_email = "\r\n\r\n--977d81ff9d852ab2a0cad646f8058349--\r\n";

/*******************************************************************************
 * Code
 ******************************************************************************/


//
//int my_smtp_bodydh_fn(void *arg, struct smtp_bodydh *bdh)
//{
//	if(bdh->state == FINISHED)
//		return BDH_DONE;
//
//	switch(bdh->state){
//	case EMAIL_START:
//		bdh->buffer = email_data_start;
//		bdh->length = EMAIL_START_LEN;
//		bdh->state = FIRST_ID;
//		break;
//	case FIRST_ID:
//		bdh->buffer = user_id;
//		bdh->length = USER_ID_LEN;
//		bdh->state = AFTER_FIRST_ID;
//		break;
//	case AFTER_FIRST_ID:
//		bdh->buffer = email_data_after_id;
//		bdh->length = AFTER_FIRST_ID_LEN;
//		bdh->state = ACCESS_TIME;
//		break;
//	case ACCESS_TIME:
//		bdh->buffer = access_time;
//		bdh->length = ACCESS_TIME_LEN;
//		bdh->state = AFTER_TIME;
//		break;
//	case AFTER_TIME:
//		bdh->buffer = email_data_after_time;
//		bdh->length = AFTER_TIME_LEN;
//		bdh->state = SECOND_ID;
//		break;
//	case SECOND_ID:
//		bdh->buffer = user_id;
//		bdh->length = USER_ID_LEN;
//		bdh->state = BET_FILENAME;
//		break;
//	case BET_FILENAME:
//		bdh->buffer = between_filename;
//		bdh->length = BET_FILENAME_LEN;
//		bdh->state = FILE_TIME;
//		break;
//	case FILE_TIME:
//		bdh->buffer = access_time_file;
//		bdh->length = FILE_TIME_LEN;
//		bdh->state = AFTER_FILE;
//		break;
//	case AFTER_FILE:
//		bdh->buffer = after_filename;
//		bdh->length = AFTER_FILE_LEN;
//		bdh->state = IMAGE;
//		break;
//	case IMAGE:
//		bdh->buffer = jpeg_base64_image;
//		bdh->length = strlen(jpeg_base64_image);
//		bdh->state = EMAIL_END;
//		break;
//	case EMAIL_END:
//		bdh->buffer = end_email;
//		bdh->length = EMAIL_END_LEN;
//		bdh->state = FINISHED;
//		break;
//	default:
//		bdh->buffer = NULL;
//		bdh->length = 0;
//		bdh->state = EMAIL_START;
//		break;
//	}
//
//    return BDH_WORKING;
//}

//this function is called when SMTP server wants to tell us something [- SMTP #2 -]
//void mySMTPresult(void *arg, u8_t smtp_result, u16_t srv_err, err_t err) {
//
//	PRINTF("mail (%p) sent with results: 0x%02x, 0x%04x, 0x%08x\n", arg,
//	smtp_result, srv_err, err);
//	/*if (email_body)
//		free(email_body);*/
//
//} //mySMTPresult

/*!
 * @brief Interrupt service for SysTick timer.
 */

/*void SysTick_Handler(void)
{
    time_isr();
}
*/

/*!
 * @brief Prints DHCP status of the interface when it has changed from last status.
 *
 * @param netif network interface structure
 */
//static void print_dhcp_state(struct netif *netif)
//{
//    static u8_t dhcp_last_state = DHCP_STATE_OFF;
//    struct dhcp *dhcp           = netif_dhcp_data(netif);
//
//    if (dhcp == NULL)
//    {
//        dhcp_last_state = DHCP_STATE_OFF;
//    }
//    else if (dhcp_last_state != dhcp->state)
//    {
//        dhcp_last_state = dhcp->state;
//
//        PRINTF(" DHCP state       : ");
//        switch (dhcp_last_state)
//        {
//            case DHCP_STATE_OFF:
//                PRINTF("OFF");
//                break;
//            case DHCP_STATE_REQUESTING:
//                PRINTF("REQUESTING");
//                break;
//            case DHCP_STATE_INIT:
//                PRINTF("INIT");
//                break;
//            case DHCP_STATE_REBOOTING:
//                PRINTF("REBOOTING");
//                break;
//            case DHCP_STATE_REBINDING:
//                PRINTF("REBINDING");
//                break;
//            case DHCP_STATE_RENEWING:
//                PRINTF("RENEWING");
//                break;
//            case DHCP_STATE_SELECTING:
//                PRINTF("SELECTING");
//                break;
//            case DHCP_STATE_INFORMING:
//                PRINTF("INFORMING");
//                break;
//            case DHCP_STATE_CHECKING:
//                PRINTF("CHECKING");
//                break;
//            case DHCP_STATE_BOUND:
//                PRINTF("BOUND");
//                break;
//            case DHCP_STATE_BACKING_OFF:
//                PRINTF("BACKING_OFF");
//                break;
//            default:
//                PRINTF("%u", dhcp_last_state);
//                assert(0);
//                break;
//        }
//        PRINTF("\r\n");
//
//        if (dhcp_last_state == DHCP_STATE_BOUND)
//        {
//            PRINTF("\r\n IPv4 Address     : %s\r\n", ipaddr_ntoa(&netif->ip_addr));
//            PRINTF(" IPv4 Subnet mask : %s\r\n", ipaddr_ntoa(&netif->netmask));
//            PRINTF(" IPv4 Gateway     : %s\r\n\r\n", ipaddr_ntoa(&netif->gw));
//        }
//    }
//}

//void send_email(void)
//{
//	smtp_set_server_addr(SERVER_IP);
//	smtp_set_server_port(SERVER_PORT);
//	smtp_set_auth(USER_AUTH, PASS_AUTH);
//
///*	int email_body_size = 50000; //Con 50000 nos aseguramos que entre un jpeg de maximo tamaño
//	email_body = malloc(email_body_size);
//	if (email_body)
//	{
//		snprintf(email_body, email_body_size, "%s%s%s%s%s%s%s%s%s%s%s", email_data_start,
//			user_id, email_data_after_id, access_time, email_data_after_time, user_id, between_filename, access_time_file, after_filename,
//			jpeg_base64_image, end_email);
//		smtp_send_mail_static(email_from, email_to, user_id, email_body, mySMTPresult, NULL);
//	}*/
//
//	smtp_send_mail_bodycback(email_from, email_to, user_id, my_smtp_bodydh_fn, mySMTPresult, NULL);
//	//smtp_send_mail_static(email_from, email_to, user_id, email_data, mySMTPresult, NULL);
//	enviar_email = 0;
//}

/*void BOARD_SW_IRQ_HANDLER(void)
{
	uint32_t flags =  GPIO_PortGetInterruptFlags(BOARD_SW3_GPIO);
	GPIO_PortClearInterruptFlags(GPIOA, flags);
	enviar_email = 1;
}*/

/*!
 * @brief Main function.
 */


int main2(void)
{
    struct netif netif;
    ip4_addr_t netif_ipaddr, netif_netmask, netif_gw;
    ethernetif_config_t enet_config = {
        .phyHandle  = &phyHandle,
        .macAddress = configMAC_ADDR,
    };

    SYSMPU_Type *base = SYSMPU;
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    /* Disable SYSMPU. */
    base->CESR &= ~SYSMPU_CESR_VLD_MASK;

    mdioHandle.resource.csrClock_Hz = EXAMPLE_CLOCK_FREQ;

    time_init();


//    gpio_pin_config_t sw_config = {
//    		kGPIO_DigitalInput, 0,
//    };
//    PORT_SetPinInterruptConfig(BOARD_SW_PORT, BOARD_SW_GPIO_PIN, kPORT_InterruptFallingEdge);
//    EnableIRQ(BOARD_SW_IRQ);
//    GPIO_PinInit(BOARD_SW_GPIO, BOARD_SW_GPIO_PIN, &sw_config);
//
//    LED_RED_INIT(LOGIC_LED_OFF);
//    LED_BLUE_INIT(LOGIC_LED_OFF);

    IP4_ADDR(&netif_ipaddr, 0U, 0U, 0U, 0U);
    IP4_ADDR(&netif_netmask, 0U, 0U, 0U, 0U);
    IP4_ADDR(&netif_gw, 0U, 0U, 0U, 0U);

    lwip_init();

    netif_add(&netif, &netif_ipaddr, &netif_netmask, &netif_gw, &enet_config, EXAMPLE_NETIF_INIT_FN, ethernet_input);
    netif_set_default(&netif);
    netif_set_up(&netif);

    dhcp_start(&netif);

    PRINTF("\r\n************************************************\r\n");
    PRINTF(" DHCP example\r\n");
    PRINTF("************************************************\r\n");

    while (1)
    {
        /* Poll the driver, get any outstanding frames */
        ethernetif_input(&netif);

        /* Handle all system timeouts for all core protocols */
        sys_check_timeouts();

        /* Print DHCP progress */
        print_dhcp_state(&netif);

        if (enviar_email == 1)
        {
        	send_email();
        }

    }
}
//#endif
