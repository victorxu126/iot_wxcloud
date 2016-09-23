#ifndef __SMARTLINK_H__
#define __SMARTLINK_H__

typedef void (* smartlink_success_callback_t)(const void *pdata);
typedef void (* smartlink_timeout_callback_t)(const void *pdata);

void smartlink_success_callback_register(smartlink_success_callback_t smartlink_success_callback);
void smartlink_timeout_callback_register(smartlink_timeout_callback_t smartlink_timeout_callback);

void smartlink_start(void);
void smartlink_stop(void);
void smartlink_test(void);

#endif /* __SMARTLINK_H__ */
