/*
 * Copyright (c) 2014 ARM. All rights reserved.
 */

#ifndef ARM_HAL_API_H_
#define ARM_HAL_API_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

/**
 * \brief This function disable global interrupts.
 */
extern void platform_enter_critical(void);
/**
 * \brief This function enable global interrupts.
 */
extern void platform_exit_critical(void);

#ifdef __cplusplus
}
#endif

#endif /* ARM_HAL_API_H_ */
