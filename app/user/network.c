/******************************************************************************
 * Copyright 2015 Vowstar Co.,Ltd.
 *
 * FileName: network.c
 *
 * Description: Network state manager
 *
 * Modification history:
 *     2015/7/22, v1.0 create this file.
*******************************************************************************/

#include "osapi.h"
#include "user_config.h"
#include "user_interface.h"
#include "network.h"

LOCAL os_timer_t network_timer;
LOCAL uint16_t network_timer_tick_count = 0;
LOCAL uint32_t network_timer_system_100ms_count = 0;
LOCAL network_state_t network_state = network_state_idle;
LOCAL network_state_changed_callback_t network_state_changed_callback_handle = NULL;
LOCAL network_system_timer_callback_t network_system_timer_callback_handle = NULL;
LOCAL bool network_state_locker = false;
LOCAL struct station_config stconfig;

void ICACHE_FLASH_ATTR
network_timer_tick()
{
	os_timer_disarm(&network_timer);
	network_timer_system_100ms_count++;
	if (0 == network_timer_system_100ms_count % 10)
	{
//		 NETWORK_DEBUG("call network_system_timer_callback_handle:%d\r\n", network_timer_system_100ms_count);
		if (network_system_timer_callback_handle)
			network_system_timer_callback_handle(network_timer_system_100ms_count / 10);
	}
	if (SOFTAP_MODE == wifi_get_opmode())
	{
		network_timer_tick_count = 0;
		network_state_change(network_state_ap);
	}
	else if ((STATION_MODE == wifi_get_opmode()) || (STATIONAP_MODE == wifi_get_opmode()))
	{
		struct ip_info ipconfig;
		wifi_get_ip_info(STATION_IF, &ipconfig);
		if (wifi_station_get_connect_status() == STATION_GOT_IP && ipconfig.ip.addr != 0) {
			network_timer_tick_count = 0;
			if (STATION_MODE == wifi_get_opmode())
			{
				network_state_change(network_state_sta_ok);
			}
			else if (STATIONAP_MODE == wifi_get_opmode())
			{
				network_state_change(network_state_ap_sta_ok);
			}
		}
		else
		{
			network_timer_tick_count++;
			if (
			    (wifi_station_get_connect_status() == STATION_IDLE) ||
			    (wifi_station_get_connect_status() == STATION_WRONG_PASSWORD) ||
			    (wifi_station_get_connect_status() == STATION_NO_AP_FOUND) ||
			    (wifi_station_get_connect_status() == STATION_CONNECT_FAIL) ||
			    (network_timer_tick_count >= 150))
			{
				if (network_timer_system_100ms_count < 300)
				{

                    if (wifi_station_get_config_default(&stconfig) == true)
                    {
                        if (os_strlen(stconfig.ssid) == 0 && os_strlen(stconfig.password) == 0)
                        {
//                          NETWORK_DEBUG("config null\r\n");
                            network_state_change(network_state_smart);
                        }
                    }
                    else
                    {
                        NETWORK_DEBUG("config error\r\n");
                    }
				}
				else
				{
					network_timer_tick_count = 0;
					if (STATION_MODE == wifi_get_opmode())
					{
						// Lock to smartcong mode
						if (network_state != network_state_smart)
							network_state_change(network_state_sta_err);
					}
					else if (STATIONAP_MODE == wifi_get_opmode())
					{
						network_state_change(network_state_ap_sta_err);
					}
				}
			}
		}
	}
	os_timer_setfn(&network_timer, (os_timer_func_t *)network_timer_tick, NULL);
	os_timer_arm(&network_timer, 100, 1);
}

void ICACHE_FLASH_ATTR
network_state_change(network_state_t change_to_state)
{
	if ((network_state != change_to_state) && (network_state_locker != true))
	{
		network_timer_tick_count = 0;
		network_state = change_to_state;
//#if defined(GLOBAL_DEBUG)
		switch (change_to_state)
		{
		case network_state_idle:
			NETWORK_DEBUG("change to network_state_idle\r\n");
			break;
		case network_state_smart:
			NETWORK_DEBUG("change to network_state_smart\r\n");
			break;
		case network_state_ap:
			NETWORK_DEBUG("change to network_state_ap\r\n");
			break;
		case network_state_ap_sta_ok:
			NETWORK_DEBUG("change to network_state_ap_sta_ok\r\n");
			break;
		case network_state_ap_sta_err:
			NETWORK_DEBUG("change to network_state_ap_sta_err\r\n");
			break;
		case network_state_sta_ok:
			NETWORK_DEBUG("change to network_state_sta_ok\r\n");
			break;
		case network_state_sta_err:
			NETWORK_DEBUG("change to network_state_sta_err\r\n");
			break;
		default:
			break;
		}
//#endif
		if (network_state_changed_callback_handle)
			network_state_changed_callback_handle(network_state);
	}
}

network_state_t ICACHE_FLASH_ATTR
network_current_state(void)
{
	return network_state;
}

void ICACHE_FLASH_ATTR
network_state_changed_callback_register(network_state_changed_callback_t network_state_changed_callback)
{
	network_state_changed_callback_handle = network_state_changed_callback;
}

void ICACHE_FLASH_ATTR
network_system_timer_callback_register(network_system_timer_callback_t network_system_timer_callback)
{
	network_system_timer_callback_handle = network_system_timer_callback;
}

void ICACHE_FLASH_ATTR
network_state_lock(void)
{
	network_state_locker = true;
}

void ICACHE_FLASH_ATTR
network_state_unlock(void)
{
	network_state_locker = false;
}


void ICACHE_FLASH_ATTR
network_init(network_state_changed_callback_t network_state_changed_callback)
{
	NETWORK_DEBUG("Init network timer\r\n");
	network_state_changed_callback_handle = network_state_changed_callback;
	network_timer_tick_count = 0;
	network_state = network_state_idle;
	os_timer_disarm(&network_timer);
	os_timer_setfn(&network_timer, (os_timer_func_t *)network_timer_tick, NULL);
	os_timer_arm(&network_timer, 100, 1);
}
