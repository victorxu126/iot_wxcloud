/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1, v1.0 create this file.
*******************************************************************************/

#include "ets_sys.h"
#include "osapi.h"

#include "user_interface.h"
#include "smartconfig.h"
#include "smartlink.h"

#include "airkiss.h"

// Select smartconfig method.
#define USE_ESPTOUCH 1
//#define USE_AIRKISS 1

LOCAL os_timer_t s_smartlink_timer;

LOCAL smartlink_success_callback_t smartlink_success_callback_handle = NULL;
LOCAL smartlink_timeout_callback_t smartlink_timeout_callback_handle = NULL;


void ICACHE_FLASH_ATTR
smartlink_timeout(void)
{
	if (smartlink_timeout_callback_handle)
	{
		smartlink_timeout_callback_handle(NULL);
	}
	os_timer_disarm(&s_smartlink_timer);
}

LOCAL void ICACHE_FLASH_ATTR
smartlink_done(sc_status status, void *pdata)
{
	switch (status)
	{
	case SC_STATUS_WAIT:
		os_printf("SC_STATUS_WAIT\n");
		break;
	case SC_STATUS_FIND_CHANNEL:
		os_printf("SC_STATUS_FIND_CHANNEL\n");
		break;
	case SC_STATUS_GETTING_SSID_PSWD:
		os_printf("SC_STATUS_GETTING_SSID_PSWD\n");
		sc_type *type = pdata;
		if (*type == SC_TYPE_ESPTOUCH)
		{
			os_printf("SC_TYPE:SC_TYPE_ESPTOUCH\n");
		}
		else
		{
			os_printf("SC_TYPE:SC_TYPE_AIRKISS\n");
		}
		break;
	case SC_STATUS_LINK:
		os_printf("SC_STATUS_LINK\n");
		struct station_config *sta_conf = pdata;

		sta_conf->bssid_set = 0;

		wifi_station_set_config(sta_conf);
		wifi_station_disconnect();
		wifi_station_connect();
		break;
	case SC_STATUS_LINK_OVER:
		os_printf("SC_STATUS_LINK_OVER\n");

		if (pdata != NULL)
		{
			uint8 phone_ip[4] = {0};

			os_memcpy(phone_ip, (uint8 *)pdata, 4);
			os_printf("Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
		}

		if (smartlink_success_callback_handle)
		{
			smartlink_success_callback_handle(pdata);
		}
		smartconfig_stop();
		break;
	}
}

void ICACHE_FLASH_ATTR
smartlink_start(void)
{
	wifi_set_opmode(STATION_MODE);
	smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS); //SC_TYPE_ESPTOUCH,SC_TYPE_AIRKISS,SC_TYPE_ESPTOUCH_AIRKISS
	
#if defined(USE_ESPTOUCH)
	// USE ESPTOUCH
	smartconfig_start(smartlink_done, 1);
#endif

#if defined(USE_AIRKISS)
	// USE AIRKISS
	smartconfig_start(smartlink_done, 1);
#endif
}

void ICACHE_FLASH_ATTR
smartlink_stop(void)
{
#if defined(USE_ESPTOUCH)
	// USE ESPTOUCH
	smartconfig_stop();
#endif

#if defined(USE_AIRKISS)
	// USE AIRKISS
	smartconfig_stop();
#endif
}

void ICACHE_FLASH_ATTR
smartlink_success_callback_register(smartlink_success_callback_t smartlink_success_callback)
{
	smartlink_success_callback_handle = smartlink_success_callback;
}

void ICACHE_FLASH_ATTR
smartlink_timeout_callback_register(smartlink_timeout_callback_t smartlink_timeout_callback)
{
	smartlink_timeout_callback_handle = smartlink_timeout_callback;
}

/**************
smartlink½á¹û
***************
Check Wi-Fi link status
STATION_IDLE
f 0, scandone
f 0, scandone
SC_STATUS_FIND_CHANNEL

TYPE: ESPTOUCH V2.4
T|PHONE MAC: a0 86 c6 96 95 2d
T|AP MAC   : 0 22 aa a5 67 88
SC_STATUS_GETTING_SSID_PSWD
SC_TYPE:SC_TYPE_ESPTOUCH
T|SCAN SSID: Test_1
T|pswd: 1234567890.
T|ssid: Test_1
T|bssid: 00 22 aa a5 67 88
SC_STATUS_LINK
f 0, scandone
add 0
aid 4
pm open phy_2,type:2 0 0
cnt

connected with Test_1, channel 2
dhcp client start...
ip:192.168.1.11,mask:255.255.255.0,gw:192.168.1.1
SC_STATUS_LINK_OVER
Phone ip: 192.168.1.32
*/


