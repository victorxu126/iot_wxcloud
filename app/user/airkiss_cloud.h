/*
 * airkiss_cloud.h
 *
 *  Created on: 2016年3月31日
 *      Author: itmaktub
 */

#ifndef APP_USER_AIRKISS_CLOUD_H_
#define APP_USER_AIRKISS_CLOUD_H_

#if defined(GLOBAL_DEBUG)
#define AIRKISS_CLOUD_DEBUG(format, ...)  log_printf("AIRKISS_CLOUD: ", format,  ##__VA_ARGS__)
#else
#define AIRKISS_CLOUD_DEBUG(format, ...)
#endif

#define AIRKISS_HEAP_MAX    1024*4

//TODO
//for DEVICE_ID     "gh_2f44589062d9_9561fcd971d28a54"
//#define DEVICE_LICENCE  "F256C41210E0B2FFAB908E90F71704856AABE7663D365E50C369862FFE015B177296D9EA1522BB929804A2EE3068988894749FAAD29B54B3330E543FD3B121DFC2F7046A84467194DFA1BB02F7D257EC"

//for DEVICE_ID     "gh_2f44589062d9_661d89e545f2abc6"
#define DEVICE_LICENCE  "D73695115D5188EC69311372B7180485D58F139AD2389C362F89D59A7FA2CA0ED2137EA1EE400D8DE3245F6C410185A1007F5842DEC79E330C45EBDD4D887C09D5E16920EFD8AAA4ECECC30AD3D3024F"

typedef enum airkiss_cloud_receive_msg_t
{
    AIRKISS_CLOUD_NOTIFY_MSG,
    AIRKISS_CLOUD_RESPONSE_MSG,
    AIRKISS_CLOUD_EVENT_MSG
}airkiss_cloud_receive_msg_t;

typedef enum airkiss_cloud_error_msg_t
{
    AIRKISS_CLOUD_ERROR_OK            = 0,                    //  0   厂家异步处理成功
    AIRKISS_CLOUD_ERROR_BUSY          = 11500,                //  1 150 0   系统繁忙
    AIRKISS_CLOUD_ERROR_NOT_CONNECT   = 11501,                //  1 150 1   设备没联网
    AIRKISS_CLOUD_ERROR_POWER_OFF     = 11502,                //  1 150 2   设备已经关机
    AIRKISS_CLOUD_ERROR_UNKOWN        = 11503                 //  1 150 3   设备暂时无法操作，请微信平台稍后重试
}airkiss_cloud_error_msg_t;

enum
{
    AIRKISS_CLOUD_FUNC_ABILITY        = 0x0001,              //    0x0001  微信硬件平台能力项业务FuncID
    AIRKISS_CLOUD_FUNC_FIRMWARE       = 0x0020                //    0x0020  微信硬件平台固件管理业务FuncID
};

uint8_t airkiss_cloud_start(void);

typedef int (* airiss_cloud_receive_notify_callback_t)(const uint8_t *dat, uint32_t len);
typedef void (* airiss_cloud_initdone_callback_t)(void);


#define airkiss_cloud_send_ablity_msg(dat)  airkiss_cloud_sendmessage(AIRKISS_CLOUD_FUNC_ABILITY, dat, os_strlen(dat));

//消息回调处理注册接口
void airiss_cloud_receive_notify_callback_register(airiss_cloud_receive_notify_callback_t airiss_cloud_receive_notify_callback);
//airkiss cloud 初始化成功后的回调注册接口
void airiss_cloud_initdone_callback_register(airiss_cloud_initdone_callback_t airiss_cloud_initdone_callback);


#endif /* APP_USER_AIRKISS_CLOUD_H_ */
