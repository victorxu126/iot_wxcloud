#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#if defined(GLOBAL_DEBUG)
#define PLATFORM_DEBUG(format, ...) log_printf("PLATFORM: ", format, ##__VA_ARGS__)
#else
#define PLATFORM_DEBUG(format, ...)
#endif

void platform_init(void);

#endif /* __PLATFORM_H__ */
