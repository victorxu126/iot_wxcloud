/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi), Modified by NodeMCU Team
 *
 * FileName: pwm.h
 *
 * Description: pwm driver
 *
 * Modification history:
 *     2014/5/1, v1.0 create this file.
*******************************************************************************/

#ifndef __SOFTPWM_H__
#define __SOFTPWM_H__

#define PWM_CHANNEL 6

struct pwm_single_param {
	uint16 gpio_set;
	uint16 gpio_clear;
	uint32 h_time;
};

struct pwm_param {
	uint32 period;
	uint16 freq;
	uint16 duty[PWM_CHANNEL];
};

#define PWM_DEPTH 1023
#define PWM_FREQ_MAX 1000

#define PWM_1S 1000000

void pwm_init(uint16 freq, uint16 *duty);
void pwm_set_duty(uint16 duty, uint8 channel);
uint16 pwm_get_duty(uint8 channel);
void pwm_set_freq(uint16 freq, uint8 channel);
uint16 pwm_get_freq(uint8 channel);
bool pwm_add(uint8 channel);
bool pwm_delete(uint8 channel);
bool pwm_exist(uint8 channel);
#endif

