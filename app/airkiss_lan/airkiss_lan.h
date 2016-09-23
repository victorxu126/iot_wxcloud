#ifndef _AIRKISS_LAN_H_
#define _AIRKISS_LAN_H_

#if defined(GLOBAL_DEBUG)
#define AIRKISS_LAN_DEBUG(format, ...) log_printf("AIRKISS_LAN: ", format,  ##__VA_ARGS__)
#else
#define AIRKISS_LAN_DEBUG(format, ...)
#endif

//TODO
#define  DEVICE_TYPE   "FIXME"
#define  DEVICE_ID     "FIXME"

#define  AIRKISS_LAN_PORT    (12476)

bool airkiss_lan_udp_create(void);
bool airkiss_lan_udp_delete(void);
void airkiss_lan_udp_broadcast_link_success_repeat(uint8_t n);

#endif
