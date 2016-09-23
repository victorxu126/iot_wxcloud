/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2015/1/23, v1.0 create this file.
*******************************************************************************/

#include "osapi.h"
#include "at_custom.h"
#include "user_interface.h"

#include "ets_sys.h"
#include "os_type.h"
#include "mem.h"
#include "espconn.h"

#include "uart.h"
#include "airkiss_app.h"
#include "my_cJSON.h"


void init_done_cb(void)
{
	os_printf("\r\ncompile time:%s %s\r\n", __DATE__, __TIME__);
	os_printf("SDK Version %s\r\n", system_get_sdk_version());

	platform_init();
}

void user_rf_pre_init(void)
{

}

void user_init(void)
{
    UART_SetPrintPort(0);
	uart_init(BIT_RATE_115200, BIT_RATE_115200);

	wifi_set_opmode(STATION_MODE);

	system_init_done_cb(init_done_cb);	//register a system-init-done callback
}

