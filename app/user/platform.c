/******************************************************************************
 * Copyright 2015 Vowstar (Shenzhen) Co.,Ltd.
 *
 * FileName: platform.c
 *
 * Description: Platform application.
 *
 * Modification history:
 *     2015/05/08, v1.0 create this file.
*******************************************************************************/

#include "osapi.h"
#include "user_config.h"
#include "user_interface.h"
#include "change_ssid.h"
#include "platform.h"
#include "uart.h"
#include "mem.h"
#include "upgrade.h"
#include "airkiss_cloudapi.h"
#include "airkiss_cloud.h"
#include "airkiss_lan.h"
#include "gpio16.h"
#include "gpio.h"
#include "softpwm.h"
#include "softpwm_light.h"
#include "network.h"
#include "airkiss_app.h"
#include "platform.h"

LOCAL bool platform_smartlink_done_flag = false;
LOCAL os_timer_t button_check_timer;

void ICACHE_FLASH_ATTR
platform_network_state_changed(network_state_t current_state)
{
	switch (current_state)
	{
	case network_state_idle:
		PLATFORM_DEBUG("stop all services\r\n");
//		airkiss_cloud_delete();
		airkiss_lan_udp_delete();
		smartlink_stop();
//		airkiss_release();
		break;
	case network_state_smart:
		PLATFORM_DEBUG("start smartlink\r\n");
//		airkiss_cloud_delete();
		airkiss_lan_udp_delete();
		smartlink_stop();
		smartlink_start();
		network_state_lock();
		break;
	case network_state_ap:
		PLATFORM_DEBUG("restart LAN services, stop cloud services\r\n");
//		airkiss_cloud_delete();
		airkiss_lan_udp_delete();
		smartlink_stop();
//		airkiss_lan_udp_create();
//		airkiss_lan_udp_server_broadcast_alive();
		break;
	case network_state_ap_sta_ok:
		PLATFORM_DEBUG("restart all services\r\n");
//		airkiss_cloud_delete();
		airkiss_lan_udp_delete();
		smartlink_stop();
		airkiss_lan_udp_create();
		airkiss_lan_udp_broadcast_link_success_repeat(10);
//		airkiss_lan_udp_server_broadcast_alive();
//		airkiss_cloud_create(airkiss_cloud_receive);
		break;
	case network_state_ap_sta_err:
		PLATFORM_DEBUG("restart LAN services, stop cloud services\r\n");
//		airkiss_cloud_delete();
//		airkiss_lan_udp_delete();
		smartlink_stop();
		airkiss_lan_udp_create();
//		airkiss_lan_udp_server_broadcast_alive();
		break;
	case network_state_sta_ok:
		PLATFORM_DEBUG("restart all services\r\n");
//		airkiss_cloud_delete();
		airkiss_lan_udp_delete();
		smartlink_stop();
		airkiss_lan_udp_create();
        airiss_cloud_receive_notify_callback_register(airkiss_app_message_receive);
        airiss_cloud_initdone_callback_register(airkiss_app_upload_status);
		airkiss_cloud_start();
		airkiss_lan_udp_broadcast_link_success_repeat(10);
		if (platform_smartlink_done_flag)
		{
//			airkiss_lan_udp_server_broadcast_link_success_repeat(10);
			platform_smartlink_done_flag = false;
		}

		break;
	case network_state_sta_err:
		PLATFORM_DEBUG("stop all services\r\n");
//		airkiss_cloud_delete();
		airkiss_lan_udp_delete();
		smartlink_stop();
		break;
	default:
		break;
	}
}

LOCAL void ICACHE_FLASH_ATTR
platform_network_smartlink_success(void * pdata)
{
	PLATFORM_DEBUG("smartlink success\r\n");
	network_state_unlock();
	platform_smartlink_done_flag = true;
}

LOCAL void ICACHE_FLASH_ATTR
platform_network_smartlink_timeout(void * pdata)
{
	PLATFORM_DEBUG("smartlink timeout\r\n");
	network_state_unlock();
	platform_smartlink_done_flag = true;
	PLATFORM_DEBUG("change to Station-AP Mode\r\n");
	wifi_set_opmode(STATIONAP_MODE);
}


void ICACHE_FLASH_ATTR
platform_init(void)
{
	pwm_init(1000, NULL);
	pwm_add(LIGHT_RED);
	pwm_add(LIGHT_GREEN);
	pwm_add(LIGHT_BLUE);
	pwm_set_duty(0, LIGHT_RED);
	pwm_set_duty(0, LIGHT_GREEN);
	pwm_set_duty(0, LIGHT_BLUE);
	pwm_start();

	airkiss_app_load();
	airkiss_app_apply_settings();
	gpio16_output_conf();
	gpio16_output_set(0);

	airkiss_app_button_init();

	os_timer_disarm(&button_check_timer);
	os_timer_setfn(&button_check_timer, (os_timer_func_t *)airkiss_app_button_check, (void *)0);
	os_timer_arm(&button_check_timer, 10, 1);

	network_init(platform_network_state_changed);
	network_system_timer_callback_register(airkiss_app_start_check);
	smartlink_success_callback_register(platform_network_smartlink_success);
	smartlink_timeout_callback_register(platform_network_smartlink_timeout);
	airkiss_app_start_check(0);
}
