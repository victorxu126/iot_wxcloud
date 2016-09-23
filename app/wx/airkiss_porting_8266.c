/*
 * airkiss_porting_8266.c
 *
 *  Created on: Dec 16, 2015
 *      Author: zorrowu
 */

#include "airkiss_porting.h"
#include "ip_addr.h"
#include "espconn.h"
#include <stdarg.h>
#include "osapi.h"
#include "mem.h"
#include "os_type.h"
#include "user_interface.h"
#include "airkiss_types.h"

#if defined(GLOBAL_DEBUG)
#define AIRKISS_PORTING_DEBUG(format, ...) log_printf("AIRKISS_PORTING:", format,  ##__VA_ARGS__)
#else
#define AIRKISS_PORTING_DEBUG(format, ...)
#endif

enum
{
	TCP_SEND_SUCCESS=0,
	TCP_SEND_FAILED=-1,
	TCP_SEND_WAITING = 2,
};

typedef struct ak_mutex_t ak_mutex_t;
static struct espconn *pairkiss_client;
static int8_t dns_state = AK_DNS_FAILED;
static int8_t tcp_state = AK_TCP_CONNECT_FAILED;
static int8_t send_state=TCP_SEND_SUCCESS;
os_timer_t wait_ip_timer;
static uint32_t Ip_addr;
//static ip_addr_t IP_addr_last;
os_timer_t dns_timer;

bool tcpclient_recvflag = 0;
uint8_t tcpclient_recvbuff[1024];
typedef struct
{
	uint8_t   rec_flag;
	uint16_t  InforLen;
	uint8_t   InforBuff[1024];
} TCPClient_msg_type;

TCPClient_msg_type TCPClient_msg;

static void tcpclient_connect_cb(void *arg);
static void tcpclient_reconnect_cb(void *arg, sint8 err);

static void ICACHE_FLASH_ATTR
tcpclient_recv(void *arg, char *pdata, unsigned short len)
{
	if(len > 0)
	{
		TCPClient_msg.rec_flag = 1;
		TCPClient_msg.InforLen = len;

		os_memcpy(TCPClient_msg.InforBuff, pdata, len);
	}
}

static void ICACHE_FLASH_ATTR
tcpclient_sent_cb(void *arg)
{
	send_state = TCP_SEND_SUCCESS;
}

static void ICACHE_FLASH_ATTR
tcpclient_disconnect_cb(void *arg)
{
    AIRKISS_PORTING_DEBUG("tcp client disconnect!\r\n");
	tcp_state = AK_TCP_CONNECT_FAILED;
}

static void ICACHE_FLASH_ATTR
tcpclient_reconnect_failed_cb(void *arg, sint8 err)
{
    AIRKISS_PORTING_DEBUG("tcp client reconnect failed !\r\n");
    tcp_state = AK_TCP_CONNECT_FAILED;
}

static void ICACHE_FLASH_ATTR
tcpclient_reconnect_cb(void *arg, sint8 err)
{

    AIRKISS_PORTING_DEBUG("tcp client reconnect!\r\n");
	tcp_state = AK_TCP_CONNECT_WAITING;

	struct espconn *conn = (struct espconn *)arg;

    espconn_regist_connectcb(pairkiss_client, tcpclient_connect_cb);
    espconn_regist_reconcb(pairkiss_client, tcpclient_reconnect_failed_cb);

    espconn_connect(pairkiss_client);


	if(err == ESPCONN_TIMEOUT)
	{
		AIRKISS_PORTING_DEBUG("tcp client error: timeout!\r\n");
	}
	else if(err == ESPCONN_ABRT)
	{
		AIRKISS_PORTING_DEBUG("tcp client error: abrt!\r\n");
	}
	else if(err == ESPCONN_RST)
	{
		AIRKISS_PORTING_DEBUG("tcp client error: reset!\r\n");
	}
	else if(err == ESPCONN_CLSD)
	{
		AIRKISS_PORTING_DEBUG("tcp client error: closed!\r\n");
	}
	else if(err == ESPCONN_CONN)
	{
		AIRKISS_PORTING_DEBUG("tcp client error: failed!\r\n");
	}
	else if(err == ESPCONN_HANDSHAKE)
	{
		AIRKISS_PORTING_DEBUG("tcp client error: handshake error!\r\n");
	}
	else
	{
		AIRKISS_PORTING_DEBUG("tcp client unknown error: %d!\r\n", err);
	}

}
static void ICACHE_FLASH_ATTR
tcpclient_connect_cb(void *arg)
{
	struct espconn *conn = (struct espconn *)arg;
	espconn_regist_disconcb(conn, tcpclient_disconnect_cb);
	espconn_regist_reconcb(conn, tcpclient_reconnect_cb);
	espconn_regist_recvcb(conn, tcpclient_recv);
	espconn_regist_sentcb(conn, tcpclient_sent_cb);

	AIRKISS_PORTING_DEBUG("tcp client connected success !\r\n");
	tcp_state = AK_TCP_CONNECT_SUCCESS;
}

int airkiss_printfImp(const char *fmt, ...)
{

}
int32_t ICACHE_FLASH_ATTR
parse_ipv4_ad(uint32_t *ip_address,    /* pointer to IP address returned */
                      char    *stringin)
{
	int error = -1;
	char    *cp;
	int   dots  =  0; /* periods imbedded in input string */
	int   number;
	union
	{
		unsigned char   c[4];
		unsigned long   l;
	} retval;

	cp = stringin;
	while (*cp)
	{
		if (*cp > '9' || *cp < '.' || *cp == '/')
		{
			return __LINE__;
		}
		if (*cp == '.')
		{
			dots++;
		}
		cp++;
	}

	if ( dots < 1 || dots > 3 )
	{
		return __LINE__;
	}

	cp = stringin;
	if ((number = atoi(cp)) > 255)
	{
		return __LINE__;
	}

	retval.c[0] = (unsigned char)number;

	while (*cp != '.')
	{
		cp++;    /* find dot (end of number) */
	}
	cp++;             /* point past dot */

	if (dots == 1 || dots == 2)
	{
		retval.c[1] = 0;
	}
	else
	{
		number = atoi(cp);
		while (*cp != '.')
		{
			cp++;    /* find dot (end of number) */
		}
		cp++;             /* point past dot */
		if (number > 255)
		{
			return (error);
		}
		retval.c[1] = (unsigned char)number;
	}

	if (dots == 1)
	{
		retval.c[2] = 0;
	}
	else
	{
		number = atoi(cp);
		while (*cp != '.')
		{
			cp++;    /* find dot (end of number) */
		}
		cp++;             /* point past dot */
		if (number > 255)
		{
			return (error);
		}
		retval.c[2] = (unsigned char)number;
	}

	if ((number = atoi(cp)) > 255)
	{
		return __LINE__;
	}
	retval.c[3] = (unsigned char)number;

	*ip_address = retval.l;
	AIRKISS_PORTING_DEBUG("dest ip is:%d.%d.%d.%d\n", retval.c[0], retval.c[1], retval.c[2], retval.c[3]);
	return 0;
}
static void ICACHE_FLASH_ATTR
dns_found_cb(const char *name, ip_addr_t *ipaddr, void *arg)
{
	struct espconn *pespconn = (struct espconn *)arg;
	static uint8_t counttime = 0;
	uint32_t tmp;
	if (ipaddr == NULL)
	{
		AIRKISS_PORTING_DEBUG("dns_found NULL\n");
		if(counttime++ > 2)
		{
		    counttime = 0;
		    os_timer_disarm(&dns_timer);
		    dns_state = AK_DNS_FAILED;
		}
	}
	else
	{
	    counttime = 0;
	    os_timer_disarm(&dns_timer);
	    //101.227.131.110
		AIRKISS_PORTING_DEBUG("dns_found  %d.%d.%d.%d\n",
			*((uint8 *)&ipaddr->addr), *((uint8 *)&ipaddr->addr + 1),
			*((uint8 *)&ipaddr->addr + 2), *((uint8 *)&ipaddr->addr + 3));

        Ip_addr = (*((uint8 *)&ipaddr->addr) << 24) | (*((uint8 *)&ipaddr->addr + 1) <<16) |
                (*((uint8 *)&ipaddr->addr + 2) << 8) | (*((uint8 *)&ipaddr->addr + 3)) ;
	}
	AIRKISS_PORTING_DEBUG("--------END dns:%ld--------\r\n", system_get_time());
}

//每隔1s查看是否dns解析成功
static void ICACHE_FLASH_ATTR
dns_check_cb(char *url)
{
    ip_addr_t ipaddr;
    AIRKISS_PORTING_DEBUG("dns_check_cb: %s\n", url);
    if(Ip_addr == 0)
        espconn_gethostbyname(pairkiss_client, url, &ipaddr, dns_found_cb);
    else
        os_timer_disarm(&dns_timer);
}

int airkiss_dns_gethost(char *url, uint32_t *ipaddr)
{
	AIRKISS_PORTING_DEBUG("url: %s\r\n",url);
    AIRKISS_PORTING_DEBUG("--------start dns:%ld--------\r\n", system_get_time());
    Ip_addr = 0;
    dns_state = AK_DNS_WAITING;
    espconn_gethostbyname(pairkiss_client, url, (ip_addr_t*)ipaddr, dns_found_cb);

    os_timer_disarm(&dns_timer);
    os_timer_setfn(&dns_timer, (os_timer_func_t *) dns_check_cb, url);
    os_timer_arm(&dns_timer, 1000, 1);
	return dns_state;
}
int airkiss_dns_checkstate(uint32_t *ipaddr)
{
    if(Ip_addr != 0)
    {
        *ipaddr = Ip_addr;
        dns_state = AK_DNS_SUCCESS;
    }
	return dns_state;
}
int airkiss_tcp_connect(ak_socket sock, char *ipaddr, uint16_t port)
{
	uint32_t addr;
	int ret;

	if (0 != parse_ipv4_ad(&addr, ipaddr))
	{
		tcp_state = AK_TCP_CONNECT_FAILED;
		AIRKISS_PORTING_DEBUG("parse ip addr error \r\n");
		return tcp_state;
	}

	tcp_state = AK_TCP_CONNECT_WAITING;

	os_memcpy(pairkiss_client->proto.tcp->remote_ip, &addr, 4);
	pairkiss_client->proto.tcp->remote_port = port;

	AIRKISS_PORTING_DEBUG("remote ip : %s:%d\r\n",ipaddr,port);

	espconn_regist_connectcb(pairkiss_client, tcpclient_connect_cb);
	espconn_regist_reconcb(pairkiss_client, tcpclient_reconnect_cb);

	espconn_connect(pairkiss_client);

	return tcp_state;
}
int airkiss_tcp_checkstate(ak_socket sock)
{
	return tcp_state;
}
int airkiss_tcp_send(ak_socket socket, char *buf, uint32_t len)
{
	if (tcp_state == AK_TCP_CONNECT_SUCCESS)
	{
		if (send_state != TCP_SEND_WAITING)
		{
			send_state = TCP_SEND_WAITING;
			espconn_send(pairkiss_client, buf, len);
		}
		return len;
	}
	else
	{
		send_state = TCP_SEND_FAILED;
		return send_state;
	}
}
void airkiss_tcp_disconnect(ak_socket socket)
{
	espconn_disconnect(pairkiss_client);
    //删除原来的连接
//    if (pairkiss_client != NULL)
//    {
//        espconn_delete(pairkiss_client);
//        if (pairkiss_client->proto.udp != NULL)
//            os_free(pairkiss_client->proto.udp);
//
//        if (pairkiss_client != NULL)
//            os_free(pairkiss_client);
//        pairkiss_client = NULL;
//    }
	AIRKISS_PORTING_DEBUG("airkiss disconnect tcp  \r\n");
}
int airkiss_tcp_recv(ak_socket socket, char *buf, uint32_t size, uint32_t timeout)
{
	int recvBytes = 0;
	int i ;
	if(TCPClient_msg.rec_flag == 1)
	{
		TCPClient_msg.rec_flag = 0;
		recvBytes = TCPClient_msg.InforLen;
		size = TCPClient_msg.InforLen;

		os_memcpy(buf, TCPClient_msg.InforBuff, size);

		AIRKISS_PORTING_DEBUG("airkiss tcp rec [ %d ] data: \r\n",size);
	}

	return recvBytes;
}
ak_socket airkiss_tcp_socket_create()
{


    AIRKISS_PORTING_DEBUG("airkiss creat tcp  \r\n");

	pairkiss_client = (struct espconn *)os_zalloc(sizeof(struct espconn));
	pairkiss_client->type = ESPCONN_TCP;
	pairkiss_client->state = ESPCONN_NONE;
	pairkiss_client->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
	pairkiss_client->proto.tcp->local_port = espconn_port();

	if (pairkiss_client != NULL)
	{
		AIRKISS_PORTING_DEBUG("tcp socket create success !\r\n");

		// 因为espconn没有描述符，使用的是espconn结构体, 这里规定airkiss_tcp_client的描述符为0
		return 0;
	}

	return -1;
}
uint32_t airkiss_gettime_ms(void)
{
	return system_get_time()/1000;
}

#ifdef AIRKISS_SUPPORT_MULTITHREAD
#include "threadx/tx_api.h"
struct ak_mutex_t
{
	TX_MUTEX m_mutex;
};

ak_mutex_t m_task_mutex;
ak_mutex_t m_malloc_mutex;
//return 0 if success
int airkiss_mutex_create(ak_mutex_t *mutex_ptr)
{
	return tx_mutex_create(&(mutex_ptr->m_mutex), "ak_mutex", TX_NO_INHERIT);
}

//return 0 if success
int airkiss_mutex_lock(ak_mutex_t *mutex_ptr)
{
	return tx_mutex_get(&(mutex_ptr->m_mutex), TX_WAIT_FOREVER);
}

//return 0 if success
int airkiss_mutex_unlock(ak_mutex_t *mutex_ptr)
{
	return tx_mutex_put(&(mutex_ptr->m_mutex));
}
#endif
