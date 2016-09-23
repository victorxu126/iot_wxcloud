#include "osapi.h"
#include "user_interface.h"
#include "change_ssid.h"
void ICACHE_FLASH_ATTR 
change_ssid()
{
#if (defined( SOFTAP_CHANGESSID ) && defined (SOFTAP_PREFIX))
    struct softap_config config;
    char macaddr[6];
    char ssid[32];

    wifi_softap_get_config(&config);
    wifi_get_macaddr(SOFTAP_IF, macaddr);

    os_sprintf(ssid, "AiThinker_%02X%02X%02X", macaddr[3], macaddr[4], macaddr[5]);
    if (os_strncmp(config.ssid, ssid, 10) == 0)
    {
        // Set SSID
        os_sprintf(ssid, SOFTAP_PREFIX "_%02X%02X%02X", macaddr[3], macaddr[4], macaddr[5]);
        os_memset(config.ssid, 0, sizeof(config.ssid));
        os_memcpy(config.ssid, ssid, os_strlen(ssid));
        config.ssid_len = os_strlen(ssid);
        wifi_softap_set_config(&config);
    }
#endif
}
