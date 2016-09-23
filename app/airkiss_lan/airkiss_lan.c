//平台相关头文件
#include "ets_sys.h"
#include "uart.h"
#include "osapi.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#include "os_type.h"
#include "mem.h"
#include "user_config.h"

//包含AirKiss头文件
#include "airkiss.h"
#include "airkiss_lan.h"


static uint8_t lan_udp_timer_counter;
static os_timer_t lan_udp_timer;
static struct espconn *pairkiss_espconn;

LOCAL uint8_t lan_buf[200];
LOCAL uint16_t lan_buf_len;

void *_memset(void *s, int ch, size_t n);
void *_memcpy(void *dest, const void *src, size_t n);
int _memcmp(const void *buf1, const void *buf2, unsigned int count);

void *_memset(void *s, int ch, size_t n)
{
    os_memset(s, ch, n);
}

void *_memcpy(void *dest, const void *src, size_t n)
{
    os_memcpy(dest, src, n);
}

int _memcmp(const void *buf1, const void *buf2, unsigned int count)
{
    os_memcmp(buf1, buf2, count);
}


static const airkiss_config_t akconf =
{
 ( airkiss_memset_fn) &_memset,
 ( airkiss_memcpy_fn) &_memcpy,
 ( airkiss_memcmp_fn) &_memcmp,
 0
};

/*
 * 平台相关定时器中断处理函数，比较正确的做法是在中断里面发送信号通知任务发送，这里
 * 为了方便说明直接发送
 */
static void ICACHE_FLASH_ATTR airkiss_lan_udp_broadcast_tick(void)
{
    airkiss_lan_ret_t ret;

    if (pairkiss_espconn != NULL)
    {
        pairkiss_espconn->proto.udp->remote_port = AIRKISS_LAN_PORT;
        // Set remote ip
        uint32_t remote_ip = 0xffffffff;
        os_memcpy(pairkiss_espconn->proto.udp->remote_ip, &remote_ip, 4);

        lan_buf_len = sizeof(lan_buf);
        ret = airkiss_lan_pack(AIRKISS_LAN_SSDP_NOTIFY_CMD,
            DEVICE_TYPE, DEVICE_ID, 0, 0, lan_buf, &lan_buf_len, &akconf);
        if (ret != AIRKISS_LAN_PAKE_READY)
        {
            AIRKISS_LAN_DEBUG("Pack lan packet error!");
            return ;
        }
        ret = espconn_sendto(pairkiss_espconn, lan_buf, lan_buf_len);
        if (ret != 0)
        {
            AIRKISS_LAN_DEBUG("UDP send error!");
        }
        AIRKISS_LAN_DEBUG("Finish send notify!\n" );
    }

    	lan_udp_timer_counter--;
    	if (lan_udp_timer_counter == 0)
    	{
    		os_timer_disarm(&lan_udp_timer);
    	}
}

/*
 * 硬件平台相关，UDP 监听端口数据接收处理函数
 */
static void ICACHE_FLASH_ATTR airkiss_lan_udp_receive(void *arg, char *pdata, unsigned short len)
{
    airkiss_lan_ret_t ret = airkiss_lan_recv(pdata, len, &akconf);
    airkiss_lan_ret_t packret;
    remot_info* pcon_info = NULL;

    switch (ret)
    {
        case  AIRKISS_LAN_SSDP_REQ:
            espconn_get_connection_info(pairkiss_espconn, &pcon_info, 0);
            AIRKISS_LAN_DEBUG("remote ip: %d.%d.%d.%d \r\n",pcon_info->remote_ip[0],pcon_info->remote_ip[1],
                pcon_info->remote_ip[2],pcon_info->remote_ip[3]);
            AIRKISS_LAN_DEBUG("remote port: %d \r\n",pcon_info->remote_port);
            pairkiss_espconn->proto.udp->remote_port = pcon_info->remote_port;
            os_memcpy(pairkiss_espconn->proto.udp->remote_ip,pcon_info->remote_ip,4);

            pairkiss_espconn->proto.udp->remote_port = AIRKISS_LAN_PORT;
            lan_buf_len = sizeof(lan_buf);
            packret = airkiss_lan_pack(AIRKISS_LAN_SSDP_RESP_CMD,
                DEVICE_TYPE, DEVICE_ID, 0, 0,
                lan_buf, &lan_buf_len, &akconf);
            if (packret != AIRKISS_LAN_PAKE_READY)
            {
                AIRKISS_LAN_DEBUG("Pack lan packet error!");
                return ;
            }
            packret = espconn_sendto(pairkiss_espconn, lan_buf, lan_buf_len);
            if (packret != 0)
            {
                AIRKISS_LAN_DEBUG("LAN UDP Send err!");
            }
            AIRKISS_LAN_DEBUG("AIRKISS_LAN_SSDP_RESP_CMD\n");
            break;
        default:
            AIRKISS_LAN_DEBUG("Pack is not ssdq req!\n");
            break;
    }
}
void ICACHE_FLASH_ATTR airkiss_lan_udp_broadcast_link_success_repeat(uint8_t n)
{
    if(lan_udp_timer_counter > 0)
        lan_udp_timer_counter = n;
    else
        lan_udp_timer_counter = 1;

    os_timer_disarm(&lan_udp_timer);
    os_timer_setfn(&lan_udp_timer, (os_timer_func_t *)airkiss_lan_udp_broadcast_tick, NULL);
    os_timer_arm(&lan_udp_timer, 3000, 1);
}

/*
 * 硬件平台相关，创建UDP 套接字，监听12476端口，与Air Kiss库使用无关
 */
bool ICACHE_FLASH_ATTR airkiss_lan_udp_create(void)
{
    if (pairkiss_espconn == NULL)
    {
        pairkiss_espconn = (struct espconn *)os_zalloc(sizeof(struct espconn));
//        return false;
    }
    AIRKISS_LAN_DEBUG("airkiss_version: %s\r\n", airkiss_version());
    AIRKISS_LAN_DEBUG("creat udp !\r\n");
    pairkiss_espconn->type = ESPCONN_UDP;
    pairkiss_espconn->proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
    pairkiss_espconn->proto.udp->local_port = AIRKISS_LAN_PORT;
    pairkiss_espconn->proto.udp->remote_port = AIRKISS_LAN_PORT;

    espconn_regist_recvcb(pairkiss_espconn, airkiss_lan_udp_receive);
    espconn_create(pairkiss_espconn);

    // Set both station and softap can get broadcast, BROADCAST_IF_STATIONAP = 3
    wifi_set_broadcast_if(3);

//    airkiss_lan_udp_broadcast_link_success_repeat(10);

    return true;
}

bool ICACHE_FLASH_ATTR airkiss_lan_udp_delete(void)
{
    AIRKISS_LAN_DEBUG("delete udp !\r\n");

    os_timer_disarm(&lan_udp_timer);
    if (pairkiss_espconn != NULL)
    {
        espconn_delete(pairkiss_espconn);
        if (pairkiss_espconn->proto.udp != NULL)
            os_free(pairkiss_espconn->proto.udp);

        if (pairkiss_espconn != NULL)
            os_free(pairkiss_espconn);
        pairkiss_espconn = NULL;
        return true;
    }
    else
    {
        return false;
    }
}
