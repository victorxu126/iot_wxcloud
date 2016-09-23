/*
 * airkiss_types.h
 *
 *  Created on: Dec 16, 2015
 *      Author: zorrowu
 */

#ifndef AIRKISS_TYPES_H_
#define AIRKISS_TYPES_H_

#ifndef ICACHE_FLASH_ATTR
#define ICACHE_FLASH_ATTR __attribute__((section(".irom0.text")))
#endif /* ICACHE_FLASH_ATTR */

#include "c_types.h"

#define ak_socket int


typedef struct ak_mutex_t ak_mutex_t;


#define ak_socket int


typedef struct ak_mutex_t ak_mutex_t;


#endif /* AIRKISS_TYPES_H_ */
