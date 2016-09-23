#ifndef __NETWORK_H__
#define __NETWORK_H__

#if defined(GLOBAL_DEBUG)
#define NETWORK_DEBUG(format, ...) log_printf("NETWORK: ", format,  ##__VA_ARGS__)
#else
#define NETWORK_DEBUG(format, ...)
#endif
typedef enum network_state_t
{
	network_state_idle,
	network_state_smart,
	network_state_ap,
	network_state_ap_sta_ok,
	network_state_ap_sta_err,
	network_state_sta_ok,
	network_state_sta_err,
} network_state_t;

typedef void (* network_state_changed_callback_t)(network_state_t current_state);
typedef void (* network_system_timer_callback_t)(uint32_t system_start_seconds);

network_state_t network_current_state(void);
void network_state_change(network_state_t change_to_state);

void network_state_changed_callback_register(network_state_changed_callback_t network_state_changed_callback);
void network_system_timer_callback_register(network_system_timer_callback_t network_system_timer_callback);
void network_init(network_state_changed_callback_t network_state_changed_callback);

void network_state_lock(void);
void network_state_unlock(void);

#endif /* __NETWORK_H__ */
