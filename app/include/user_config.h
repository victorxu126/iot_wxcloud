#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define ICACHE_STORE_TYPEDEF_ATTR __attribute__((aligned(4),packed))
#define ICACHE_STORE_ATTR __attribute__((aligned(4)))
#define ICACHE_RAM_ATTR

#define USE_OPTIMIZE_PRINTF

#define SOFTAP_CHANGESSID 	1
#define SOFTAP_PREFIX 		"AiThinker"
#define WDT_CLEAR()			WRITE_PERI_REG( 0x60000914, 0x73 )

#define log_printf(header, fmt, ...) do {    \
    static const char flash_str[] ICACHE_RODATA_ATTR __attribute__((aligned(4))) = fmt; \
    if(os_strlen(fmt) > 2) \
    { \
        static const char header_str[] ICACHE_RODATA_ATTR __attribute__((aligned(4))) = header; \
        os_printf_plus(header_str, ##__VA_ARGS__);   \
    } \
    os_printf_plus(flash_str, ##__VA_ARGS__);   \
    } while(0)

#ifndef ICACHE_FLASH 
#define ICACHE_FLASH
#endif

#endif
