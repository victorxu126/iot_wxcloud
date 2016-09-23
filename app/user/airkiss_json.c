/*
 * airkiss_json.c
 *
 *  Created on: 2016Äê4ÔÂ1ÈÕ
 *      Author: itmaktub
 */

#include "c_types.h"
#include "mem.h"
#include "osapi.h"
#include "os_type.h"
#include "my_cJSON.h"
#include "airkiss_json.h"


int ICACHE_FLASH_ATTR airkiss_json_ask_set(char * _buf, uint64_t _msg_id, uint8_t _operation_status, uint8_t _power_switch, uint8_t alpha)
{
    my_cJSON *root;
    /*create json string root*/
    root = my_cJSON_CreateObject();
    if(!root)
    {
        AIRKISS_JSON_DEBUG("get root faild !\n");
        return -1;
    }

    my_cJSON * js_services ;
    my_cJSON * js_operation_status ;
    my_cJSON * js_power_switch ;
    my_cJSON * js_lightbulb ;

    my_cJSON_AddNumberToObject(root, "asy_error_code",0);
    my_cJSON_AddStringToObject(root, "asy_error_msg", "ok");
    my_cJSON_AddNumberToObject(root, "msg_id", _msg_id);
    my_cJSON_AddStringToObject(root, "msg_type", "set");


    const char *const servers = "services";
    const char *const operation_status = "operation_status";
    const char *const power_switch = "power_switch";
    const char *const lightbulb = "lightbulb";


    my_cJSON_AddItemToObject(root, servers, js_services=my_cJSON_CreateObject());

    my_cJSON_AddItemToObject(js_services, operation_status, js_operation_status=my_cJSON_CreateObject());
    if(_operation_status >= 0 && _operation_status <= 2)
    {
        my_cJSON_AddNumberToObject(js_operation_status,"status", _operation_status);
    }
    else
    {
        AIRKISS_JSON_DEBUG("status error, user default ! \r\n");
        my_cJSON_AddNumberToObject(js_operation_status,"status", 2);
    }

    my_cJSON_AddItemToObject(js_services, power_switch, js_power_switch=my_cJSON_CreateObject());
    if(_power_switch == 0)
    {
        my_cJSON_AddFalseToObject(js_power_switch,"on_off");
    }
    else
    {
        my_cJSON_AddTrueToObject(js_power_switch,"on_off");
    }

    my_cJSON_AddItemToObject(js_services, lightbulb, js_lightbulb=my_cJSON_CreateObject());


    my_cJSON_AddNumberToObject(js_lightbulb, "alpha", alpha);

    char *s = my_cJSON_PrintUnformatted(root);
    if(s)
    {
//        os_memset(_buf, 0, sizeof(_buf));
        os_memcpy(_buf, s, os_strlen(s));
        AIRKISS_JSON_DEBUG("airkiss_json_ask_set:\r\n %s\n",_buf);
        os_free(s);
    }
    my_cJSON_Delete(root);
    return 0;
}

//TODO
int ICACHE_FLASH_ATTR airkiss_json_ask_get(char * _buf, uint64_t _msg_id, char *_user, uint8_t _operation_status, uint8_t _power_switch, uint8_t _alpha)
{
    my_cJSON *root;
    my_cJSON * js_services ;
    my_cJSON * js_operation_status ;
    my_cJSON * js_power_switch ;
    my_cJSON * js_lightbulb ;
    /*create json string root*/
    root = my_cJSON_CreateObject();
    if(!root)
    {
        AIRKISS_JSON_DEBUG("get root faild !\n");
        return -1;
    }
    my_cJSON_AddNumberToObject(root, "msg_id", _msg_id);
    my_cJSON_AddStringToObject(root, "user", _user);
    my_cJSON_AddStringToObject(root, "msg_type", "get");

    const char *const servers = "services";
    const char *const operation_status = "operation_status";
    const char *const power_switch = "power_switch";
    const char *const lightbulb = "lightbulb";

    my_cJSON_AddItemToObject(root, servers, js_services=my_cJSON_CreateObject());
    my_cJSON_AddItemToObject(js_services, operation_status, js_operation_status=my_cJSON_CreateObject());
    my_cJSON_AddItemToObject(js_services, power_switch, js_power_switch=my_cJSON_CreateObject());
    my_cJSON_AddItemToObject(js_services, lightbulb, js_lightbulb=my_cJSON_CreateObject());

    if(_operation_status >= 0 && _operation_status <= 2)
    {
        my_cJSON_AddNumberToObject(js_operation_status,"status", _operation_status);
    }
    else
    {
        AIRKISS_JSON_DEBUG("status error, user default ! \r\n");
        my_cJSON_AddNumberToObject(js_operation_status,"status", 1);
    }

    if(_power_switch == 0)
    {
        my_cJSON_AddFalseToObject(js_power_switch, "on_off");
    }
    else
    {
        my_cJSON_AddTrueToObject(js_power_switch, "on_off");
    }

    my_cJSON_AddNumberToObject(js_lightbulb, "alpha", _alpha);

    char *s = my_cJSON_PrintUnformatted(root);
    if(s)
    {
//        os_memset(_buf, 0, sizeof(_buf));
        os_memcpy(_buf, s, os_strlen(s));
        AIRKISS_JSON_DEBUG("airkiss_json_ask_get:\r\n %s\n",_buf);
        os_free(s);
    }
    my_cJSON_Delete(root);
    return 0;
}

int ICACHE_FLASH_ATTR airkiss_json_upload(char * _buf, uint8_t _operation_status, uint8_t _power_switch, uint8_t _alpha)
{
    my_cJSON *root;
    my_cJSON * js_services ;
    my_cJSON * js_operation_status ;
    my_cJSON * js_power_switch ;
    my_cJSON * js_lightbulb ;
    /*create json string root*/
    root = my_cJSON_CreateObject();
    if(!root)
    {
        AIRKISS_JSON_DEBUG("get root faild !\n");
        return -1;
    }

    my_cJSON_AddStringToObject(root, "msg_type", "notify");

    const char *const servers = "services";
    const char *const operation_status = "operation_status";
    const char *const power_switch = "power_switch";
    const char *const lightbulb = "lightbulb";

    my_cJSON_AddItemToObject(root, servers, js_services=my_cJSON_CreateObject());
    my_cJSON_AddItemToObject(js_services, operation_status, js_operation_status=my_cJSON_CreateObject());
    my_cJSON_AddItemToObject(js_services, power_switch, js_power_switch=my_cJSON_CreateObject());
    my_cJSON_AddItemToObject(js_services, lightbulb, js_lightbulb=my_cJSON_CreateObject());

    if(_operation_status >= 0 && _operation_status <= 2)
    {
        my_cJSON_AddNumberToObject(js_operation_status,"status", _operation_status);
    }
    else
    {
        AIRKISS_JSON_DEBUG("status error, user default ! \r\n");
        my_cJSON_AddNumberToObject(js_operation_status,"status", 1);
    }

    if(_power_switch == 0)
    {
        my_cJSON_AddFalseToObject(js_power_switch, "on_off");
    }
    else
    {
        my_cJSON_AddTrueToObject(js_power_switch, "on_off");
    }

    my_cJSON_AddNumberToObject(js_lightbulb, "alpha", _alpha);

    char *s = my_cJSON_PrintUnformatted(root);
    if(s)
    {
//        os_memset(_buf, 0, sizeof(_buf));
        os_memcpy(_buf, s, os_strlen(s));
        AIRKISS_JSON_DEBUG("airkiss_json_upload:\r\n %s\r\n",_buf);
        os_free(s);
    }
    my_cJSON_Delete(root);
    return 0;
}
