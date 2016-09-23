/*
 * airkiss_app.h
 *
 *  Created on: 2016Äê3ÔÂ31ÈÕ
 *      Author: itmaktub
 */

#ifndef APP_USER_AIRKISS_APP_H_
#define APP_USER_AIRKISS_APP_H_

#if defined(GLOBAL_DEBUG)
#define AIRKISS_APP_DEBUG(format, ...)  log_printf("AIRKISS_APP: ", format,  ##__VA_ARGS__)
#else
#define AIRKISS_APP_DEBUG(format, ...)
#endif

#define AIRKISS_APP_START_SEC   0x90

typedef struct mcu_status_t
{
        uint8_t status;
        uint8_t power_switch;
        uint8_t alpha;
} __attribute__((aligned(1), packed)) mcu_status_t;

typedef struct system_status_t
{
    uint8_t init_flag;
    uint16_t start_count;
    uint8_t start_continue;
    mcu_status_t mcu_status;
} __attribute__((aligned(4), packed)) system_status_t;

typedef enum airkiss_app_state_t
{
    airkiss_app_state_normal,
    airkiss_app_state_smart,
    airkiss_app_state_upgrade,
    airkiss_app_state_restore,
} airkiss_app_state_t;

typedef enum
{
    airkiss_system_state_run,
    airkiss_system_state_supend,
    airkiss_system_state_off
} airkiss_system_state_t;

typedef enum
{
    airkiss_power_state_off,
    airkiss_power_state_on
} airkiss_power_state_t;


void airkiss_app_apply_settings(void);
void airkiss_app_load(void);
void airkiss_app_save(void);
void airkiss_app_upload_status();

void airkiss_app_button_check(void);
void airkiss_app_button_init(void);
void airkiss_app_start_check(uint32_t system_start_seconds);
int airkiss_app_message_receive(const uint8_t *_data, uint32_t _datalen);

#endif /* APP_USER_AIRKISS_APP_H_ */
