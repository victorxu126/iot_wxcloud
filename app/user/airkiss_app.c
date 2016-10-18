/*
 * airkiss_app.c
 *
 *  Created on: 2016年3月31日
 *      Author: itmaktub
 */
#include "osapi.h"
#include "user_config.h"
#include "user_interface.h"
#include "network.h"
#include "gpio.h"
#include "color.h"

#include "softpwm.h"
#include "softpwm_light.h"

#include "airkiss_app.h"
#include "airkiss_json.h"
#include "airkiss_cloud.h"
#include "my_cJSON.h"
#include "mem.h"
#include "pin_map.h"


#define PRESS 			0
#define UNPRESS 		1

#define KEY_IO_NUM     	4		//airkiss control button use GPIO4
#define KEY_IO_MUX     	pin_name[KEY_IO_NUM]
#define KEY_IO_FUNC    	pin_func[KEY_IO_NUM]
#define LED16_IO_NUM    12		//LED D16 control use GPIO9
#define LED16_IO_MUX    pin_name[LED16_IO_NUM]
#define LED16_IO_FUNC   pin_func[LED16_IO_NUM]
#define LED4_IO_NUM		14		//LED D4 control use GPIO10
#define LED4_IO_MUX		pin_name[LED4_IO_NUM]
#define LED4_IO_FUNC	pin_func[LED4_IO_NUM]

uint8_t airkiss_json_send_buffer[256];

LOCAL mcu_status_t local_mcu_status = {
    .status = airkiss_system_state_run,
    .power_switch = airkiss_power_state_on,
    .alpha = 100
};

LOCAL system_status_t local_system_status = {
    .init_flag = 0,
    .start_count = 0,
    .start_continue = 0,
    .mcu_status = {
       .status = airkiss_system_state_run,
       .power_switch = airkiss_power_state_on,
       .alpha = 100
    },
};

LOCAL airkiss_app_state_t airkiss_app_state = airkiss_app_state_normal;
LOCAL os_timer_t airkiss_app_smart_timer;



void ICACHE_FLASH_ATTR airkiss_app_button_event()
{
    if(local_mcu_status.status == airkiss_system_state_run)
        local_mcu_status.status = airkiss_system_state_supend;
    else
        local_mcu_status.status = airkiss_system_state_run;
    airkiss_app_apply_settings();
    airkiss_app_save();
    airkiss_app_upload_status();
}

void ICACHE_FLASH_ATTR airkiss_app_upload_status()
{
    os_memset(airkiss_json_send_buffer, 0, sizeof(airkiss_json_send_buffer));

    int ret = airkiss_json_upload(
        airkiss_json_send_buffer,
        local_mcu_status.status,
        local_mcu_status.power_switch,
        local_mcu_status.alpha);
    if(ret == 0)
        airkiss_cloud_send_ablity_msg(airkiss_json_send_buffer);
}

void ICACHE_FLASH_ATTR
airkiss_app_button_init(void)
{
	PIN_FUNC_SELECT(KEY_IO_MUX, KEY_IO_FUNC);

	GPIO_DIS_OUTPUT(4);

}

void ICACHE_FLASH_ATTR
airkiss_app_led_init(void)
{
	PIN_FUNC_SELECT(LED16_IO_MUX, LED16_IO_FUNC);
	PIN_FUNC_SELECT(LED4_IO_MUX, LED4_IO_FUNC);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(LED16_IO_NUM),0);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(LED4_IO_NUM),0);
}

void ICACHE_FLASH_ATTR
airkiss_app_led_check(void)
{
	static u8 count = 0;
	count = (count+1)%2;

//	AIRKISS_APP_DEBUG("airkiss_app_led_check!,count=%d\r\n",count);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(LED16_IO_NUM),count);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(LED4_IO_NUM),count);

}

void ICACHE_FLASH_ATTR
airkiss_app_alarming(void)
{
	static u8 count = 0;
	count = (count+1)%2;

	GPIO_OUTPUT_SET(GPIO_ID_PIN(LED4_IO_NUM),count);

}

//按键扫描,10ms调度一次
void ICACHE_FLASH_ATTR
airkiss_app_button_check(void)
{
    static u8 KEY_State = 0;
    static u16 KEY_TimeCount = 0;

    switch (KEY_State)
    {
        case 0:
            if (PRESS == GPIO_INPUT_GET(KEY_IO_NUM))
            {
                KEY_State ++;
            }
            break;
        case 1:
            if (PRESS == GPIO_INPUT_GET(KEY_IO_NUM)) //确认按键按下
            {
                //KeyValue=1;
                KEY_TimeCount = 0;
                KEY_State ++;
            }
            else //干扰
            {
                KEY_State = 0;
            }
            break;
        case 2:
            if (KEY_TimeCount <= 3*1000/10) //3s
            {
                KEY_TimeCount++;
            }
            else
            {
                AIRKISS_APP_DEBUG("button long press!!!\r\n");
                network_state_change(network_state_smart);	//start airkiss
                KEY_State ++;
            }

            if (UNPRESS == GPIO_INPUT_GET(KEY_IO_NUM)) //松手
            {
                KEY_State ++;
            }
            break;
        case 3:
            if (UNPRESS == GPIO_INPUT_GET(KEY_IO_NUM)) //松手
            {
                if (KEY_TimeCount >= 100/10)			//至少100ms
                {
                    if ((KEY_TimeCount < 2*1000/10))			//1S
                    {
//                        KeyValue = KEY_1S;
                        AIRKISS_APP_DEBUG("button press!!!\r\n");
                        airkiss_app_button_event();
                    }
                }
                KEY_State = 0;
            }
            break;
        default:
            break;
    }
}


int ICACHE_FLASH_ATTR
airkiss_app_message_receive(const uint8_t *_data, uint32_t _datalen)
{
//    airkiss_app_msg_t airkiss_app_msg;
    my_cJSON *root = NULL;
    my_cJSON *js_msg_id = NULL;
    my_cJSON *js_msg_type = NULL;
    my_cJSON *js_user = NULL;
    my_cJSON *js_services = NULL;
    my_cJSON *js_operation_status = NULL;
    my_cJSON *js_status = NULL;
    my_cJSON *js_power_switch = NULL;
    my_cJSON *js_on_off = NULL;
    my_cJSON *js_lightbulb = NULL;
    my_cJSON *js_alpha = NULL;
    my_cJSON *js_alarm_notify = NULL;
    my_cJSON *js_notify_type = NULL;
    char *s = NULL;
    int ret = -1;

    if(_datalen)
    {
        //必备属性
        root = my_cJSON_Parse(_data);
        if(!root) { AIRKISS_APP_DEBUG("get root faild !\n"); return -1; }

        js_msg_id = my_cJSON_GetObjectItem(root, "msg_id");
        AIRKISS_APP_DEBUG("get msg_id = %d !\n",js_msg_id->valueint);
        if(!js_msg_id) { AIRKISS_APP_DEBUG("get msg_id faild !\n"); return -1; }

        js_msg_type = my_cJSON_GetObjectItem(root, "msg_type");
        if(!js_msg_type) { AIRKISS_APP_DEBUG("get msg_type faild !\n"); return -1; }

        js_user = my_cJSON_GetObjectItem(root, "user");
        if(!js_user) { AIRKISS_APP_DEBUG("get user faild !\n"); return -1; }

        js_services = my_cJSON_GetObjectItem(root, "services");
        if(!js_services) { AIRKISS_APP_DEBUG("get services faild !\n"); return -1; }

        js_operation_status = my_cJSON_GetObjectItem(js_services, "operation_status");
        if(!js_operation_status) { AIRKISS_APP_DEBUG("get operation_status faild !\n"); return -1; }

        js_status = my_cJSON_GetObjectItem(js_operation_status, "status");
        if(!js_status) { AIRKISS_APP_DEBUG("get status faild !\n"); return -1; }

//        os_memcpy(airkiss_app_msg.msg_type, js_msg_type->valuestring, os_strlen(js_msg_type->valuestring));
//        os_memcpy(airkiss_app_msg.user, js_user->valuestring, os_strlen(js_user->valuestring));
//        airkiss_app_msg.msg_id = js_msg_id->valueint;
//        airkiss_app_msg.status = js_status->valueint;

        local_mcu_status.status = (uint8_t) js_status->valueint;


#if 0
        //非必备属性
        //开关
        js_power_switch = my_cJSON_GetObjectItem(js_services, "power_switch");
        if(js_power_switch)
        {
            js_on_off = my_cJSON_GetObjectItem(js_power_switch, "on_off");
            if(!js_on_off)
            {
                AIRKISS_APP_DEBUG("no on_off!\n");
                return -1;
            }
            local_mcu_status.power_switch = (js_on_off->type == my_cJSON_False)?airkiss_power_state_off:airkiss_power_state_on;
//            airkiss_app_msg.power_switch = (js_on_off->type == my_cJSON_False)?0:1;
        }
        else
        {
            AIRKISS_APP_DEBUG("get power_switch faild !\n");
        }

        //灯能力属性
        js_lightbulb = my_cJSON_GetObjectItem(js_services, "lightbulb");
        if(js_lightbulb)
        {
            js_alpha = my_cJSON_GetObjectItem(js_lightbulb, "alpha");
            if(!js_alpha)
            {
                AIRKISS_APP_DEBUG("no alpha!\n");
                return -1;
            }

            if(js_alpha->valueint <= 100 && js_alpha->valueint >= 0 )
                local_mcu_status.alpha = js_alpha->valueint;
        }
        else
        {
            AIRKISS_APP_DEBUG("get lightbulb faild !\n");
        }
#else
        //报警器控制
        //煤气开关
		js_power_switch = my_cJSON_GetObjectItem(js_services, "power_switch");
		if(js_power_switch)
		{
			js_on_off = my_cJSON_GetObjectItem(js_power_switch, "on_off");
			if(!js_on_off)
			{
				AIRKISS_APP_DEBUG("no on_off!\n");
				return -1;
			}
			local_mcu_status.power_switch = (js_on_off->type == my_cJSON_False)?airkiss_power_state_off:airkiss_power_state_on;
//            airkiss_app_msg.power_switch = (js_on_off->type == my_cJSON_False)?0:1;
		}
		else
		{
			AIRKISS_APP_DEBUG("get power_switch failed !\n");
		}
		//强制报警属性
		js_alarm_notify = my_cJSON_GetObjectItem(js_services, "notify");
		if(js_alarm_notify)
		{
			js_notify_type = my_cJSON_GetObjectItem(js_alarm_notify, "notify_type");
			if(!js_notify_type)
			{
				AIRKISS_APP_DEBUG("no notify_type parameters!\n");
				return -1;
			}

			local_mcu_status.alpha = js_notify_type->valueint;
		}
		else
		{
			AIRKISS_APP_DEBUG("get notify_type faild !\n");
		}
#endif


        my_cJSON_Delete(root);

        //处理信息
        airkiss_app_apply_settings();
//        airkiss_app_save();

        AIRKISS_APP_DEBUG("msg_type: %s\n", js_msg_type->valuestring);
        //回复信息
        if(os_strcmp(js_msg_type->valuestring, "set", os_strlen("set")) == 0)
        {
            os_memset(airkiss_json_send_buffer, 0, sizeof(airkiss_json_send_buffer));
            ret = airkiss_json_ask_set(airkiss_json_send_buffer, js_msg_id->valueint, local_mcu_status.status, local_mcu_status.power_switch, local_mcu_status.alpha);
        }
        else if(os_strcmp(js_msg_type->valuestring, "get", os_strlen("get")) == 0)
        {
            os_memset(airkiss_json_send_buffer, 0, sizeof(airkiss_json_send_buffer));
//            ret = airkiss_json_ask_get(airkiss_json_send_buffer, js_msg_id->valueint, js_user->valuestring, local_mcu_status.status, local_mcu_status.power_switch, local_mcu_status.alpha);
            ret = airkiss_json_ask_get_gas(airkiss_json_send_buffer, js_msg_id->valueint, js_user->valuestring, local_mcu_status.status);
        }

        if(ret == 0) airkiss_cloud_send_ablity_msg(airkiss_json_send_buffer);
    }
    return 0;
}

LOCAL os_timer_t alarm_timer;
void airkiss_app_apply_settings(void)
{
	if ( local_mcu_status.power_switch == airkiss_power_state_off){
		GPIO_OUTPUT_SET(GPIO_ID_PIN(LED16_IO_NUM),0);
	}else{
		GPIO_OUTPUT_SET(GPIO_ID_PIN(LED16_IO_NUM),1);
	}

	if(local_mcu_status.alpha ==1){
		os_timer_disarm(&alarm_timer);
		os_timer_setfn(&alarm_timer, (os_timer_func_t *)airkiss_app_alarming, (void *)0);
		os_timer_arm(&alarm_timer, 500, 1);
	}else{
		os_timer_disarm(&alarm_timer);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(LED4_IO_NUM),0);
	}
    /*if (local_mcu_status.status == airkiss_system_state_run)
    {

        if(local_mcu_status.alpha > 100)
            local_mcu_status.alpha = 100;

        AIRKISS_APP_DEBUG("alpha: %d\r\n", local_mcu_status.alpha);

        pwm_set_duty((uint32_t) (1023) * local_mcu_status.alpha / 100, LIGHT_RED);
        pwm_set_duty((uint32_t) (1023) * local_mcu_status.alpha / 100, LIGHT_GREEN);
        pwm_set_duty((uint32_t) (1023) * local_mcu_status.alpha / 100, LIGHT_BLUE);
    }
    else
    {
        pwm_set_duty(0, LIGHT_RED);
        pwm_set_duty(0, LIGHT_GREEN);
        pwm_set_duty(0, LIGHT_BLUE);
    }
    pwm_start();
    */
}

void airkiss_app_load(void)
{
    system_param_load(				//Read protected data from flash
        (AIRKISS_APP_START_SEC),
        0,
        (void *)(&local_system_status),
        sizeof(local_system_status));
    if (local_system_status.init_flag)
    {
        local_mcu_status = local_system_status.mcu_status;
    }
    else
    {
        local_system_status.init_flag = 1;
    }
    local_system_status.start_count += 1;	//启动次数
    local_system_status.start_continue += 1;
    AIRKISS_APP_DEBUG("start count:%d,start_continue:%d\r\n", local_system_status.start_count, local_system_status.start_continue);
    airkiss_app_save();
}

void airkiss_app_save(void)
{
    local_system_status.mcu_status = local_mcu_status;
    system_param_save_with_protect(		//Write data into flash with protection
        (AIRKISS_APP_START_SEC),
        (void *)(&local_system_status),
        sizeof(local_system_status));
    AIRKISS_APP_DEBUG("mcu_stauts data: .status= %d, .power_switch= %d, .alpha=%d\r\n",local_mcu_status.status,local_mcu_status.power_switch,local_mcu_status.alpha);
    AIRKISS_APP_DEBUG("param saved !\r\n");
}

void ICACHE_FLASH_ATTR
airkiss_app_smart_timer_tick()
{
    static double h = 0;
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    os_timer_disarm(&airkiss_app_smart_timer);
    os_timer_setfn(&airkiss_app_smart_timer, (os_timer_func_t *)airkiss_app_smart_timer_tick, NULL);
    os_timer_arm(&airkiss_app_smart_timer, 100, 1);
    h += 5;
    if (h >= 360)
        h = 0;
    HLS2RGB( &r, &g, &b, h, 0.3, 1);

    pwm_set_duty((uint32_t)(r) * (1023) * local_mcu_status.alpha / 100 / 255, LIGHT_RED);
//    pwm_set_duty((uint32_t)(g) * (1023) * local_mcu_status.alpha / 100 / 255, LIGHT_GREEN);
//    pwm_set_duty((uint32_t)(b) * (1023) * local_mcu_status.alpha / 100 / 255, LIGHT_BLUE);
    pwm_start();
}

void ICACHE_FLASH_ATTR
airkiss_app_start_check(uint32_t system_start_seconds)
{
    if ((local_system_status.start_continue != 0) && (system_start_seconds > 5))
    {
        local_system_status.start_continue = 0;
        airkiss_app_save();
        AIRKISS_APP_DEBUG("airkiss_app_start_check ---1 \r\n");
    }

    if (local_system_status.start_continue >= 10)
    {
        if (airkiss_app_state_restore != airkiss_app_state)
        {
            AIRKISS_APP_DEBUG("system restore\r\n");
            airkiss_app_state = airkiss_app_state_restore;
            // Init flag and counter
            local_system_status.init_flag = 0;
            local_system_status.start_continue = 0;
            // Save param
            airkiss_app_save();
            // Restore system, reset to default settings
            system_restore();
            // Restart system
            system_restart();
            AIRKISS_APP_DEBUG("airkiss_app_start_check ---2 \r\n");
        }
    }
    else if (local_system_status.start_continue >= 3)
    {
        network_state_change(network_state_smart);
        AIRKISS_APP_DEBUG("airkiss_app_start_check ---3 \r\n");
    }

    if (network_state_smart == network_current_state())
    {
        if (airkiss_app_state_smart != airkiss_app_state)
        {
            AIRKISS_APP_DEBUG("begin smart config effect\r\n");
            airkiss_app_state = airkiss_app_state_smart;
            os_timer_disarm(&airkiss_app_smart_timer);
            os_timer_setfn(&airkiss_app_smart_timer, (os_timer_func_t *)airkiss_app_smart_timer_tick, NULL);
            os_timer_arm(&airkiss_app_smart_timer, 100, 1);
            AIRKISS_APP_DEBUG("airkiss_app_start_check ---4 \r\n");
        }
    }
    else
    {
        if (airkiss_app_state_smart == airkiss_app_state)
        {
            airkiss_app_state = airkiss_app_state_normal;
            airkiss_app_apply_settings();
            os_timer_disarm(&airkiss_app_smart_timer);
            AIRKISS_APP_DEBUG("airkiss_app_start_check ---5 \r\n");
        }
    }

}
