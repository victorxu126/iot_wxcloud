/*
 * airkiss_cloud.c
 *
 *  Created on: 2016Äê3ÔÂ31ÈÕ
 *      Author: itmaktub
 */

#include "c_types.h"
#include "mem.h"
#include "osapi.h"
#include "os_type.h"
#include "my_cJSON.h"

#include "airkiss_cloud.h"
#include "airkiss_cloudapi.h"


uint32_t heapbuf[AIRKISS_HEAP_MAX];//4k for airkiss heap

LOCAL os_timer_t wx_cloud_loop_timer;
LOCAL airiss_cloud_receive_notify_callback_t airiss_cloud_receive_notify_callback_handle = NULL;
LOCAL airiss_cloud_initdone_callback_t airiss_cloud_initdone_callback_handle = NULL;

static void ICACHE_FLASH_ATTR
printfstrlen(const uint8_t *body, uint32_t bodylen)
{
	int i;
	for (i=0; i<bodylen; i++)
	{
	    AIRKISS_CLOUD_DEBUG("%c", body[i]);
	}
}

void ICACHE_FLASH_ATTR
airiss_cloud_receive_notify_callback_register(airiss_cloud_receive_notify_callback_t airiss_cloud_receive_notify_callback)
{
    airiss_cloud_receive_notify_callback_handle = airiss_cloud_receive_notify_callback;
}

void ICACHE_FLASH_ATTR
airiss_cloud_initdone_callback_register(airiss_cloud_initdone_callback_t airiss_cloud_initdone_callback)
{
    airiss_cloud_initdone_callback_handle = airiss_cloud_initdone_callback;
}

static void ICACHE_FLASH_ATTR
ReceiveNotifyCB(uint32_t funcid, const uint8_t *body, uint32_t bodylen)
{
	AIRKISS_CLOUD_DEBUG("Recv notify Callback funcid:%d, len:%d\r\n", funcid, bodylen);
	printfstrlen(body, bodylen);
	AIRKISS_CLOUD_DEBUG("\r\n");

	if(airiss_cloud_receive_notify_callback_handle != NULL)
	    airiss_cloud_receive_notify_callback_handle( body, bodylen);
}

static void ICACHE_FLASH_ATTR
ReceiveResponseCB(uint32_t taskid, uint32_t errcode, uint32_t funcid, const uint8_t *body, uint32_t bodylen)
{
	AIRKISS_CLOUD_DEBUG("Enter Resp Callback:id:%d, err:%d, funcid:%d, len:%d\r\n", taskid, errcode, funcid, bodylen);
	printfstrlen(body, bodylen);
	AIRKISS_CLOUD_DEBUG("\r\n");
}

static void ICACHE_FLASH_ATTR
ReceiveEventCB(EventValue event_value)
{
	AIRKISS_CLOUD_DEBUG("Recv Event Callback:%d\r\n", event_value);

	switch (event_value)
	{
	case EVENT_VALUE_LOGIN:
		AIRKISS_CLOUD_DEBUG("Device Login!\r\n");
		break;
	case EVENT_VALUE_LOGOUT:
		AIRKISS_CLOUD_DEBUG("Device Logout!\r\n");
		break;
	}
}


uint8_t ICACHE_FLASH_ATTR
airkiss_cloud_start(void)
{
	AIRKISS_CLOUD_DEBUG("Airkiss lib version:%s\n", airkiss_cloud_version());
	

	if (0 != airkiss_cloud_init((uint8_t *)DEVICE_LICENCE,
		(uint32_t)os_strlen((const char *)DEVICE_LICENCE),
		0, 0,
		heapbuf, sizeof(heapbuf)))
	{
	    AIRKISS_CLOUD_DEBUG("SDK init failed!!!\r\n");
		return 1;
	}
	
	//regist callback functions
	airkiss_callbacks_t cbs;
	cbs.m_notifycb = ReceiveNotifyCB;
	cbs.m_respcb = ReceiveResponseCB;
	cbs.m_eventcb = ReceiveEventCB;
	airkiss_regist_callbacks(&cbs);

	//upload data when init done
	if(airiss_cloud_initdone_callback_handle != NULL)
	    airiss_cloud_initdone_callback_handle();

	//system loop
	os_timer_disarm(&wx_cloud_loop_timer);
	os_timer_setfn(&wx_cloud_loop_timer, (os_timer_func_t *)airkiss_cloud_loop, 0);
	os_timer_arm(&wx_cloud_loop_timer, 100, 1);

	return 0;
}




