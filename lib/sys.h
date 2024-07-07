/*! \copyright 2024 Zorxx Software. All rights reserved.
 *  \license This file is released under the MIT License. See the LICENSE file for details.
 *  \brief lowlevel interface
 */
#ifndef _SYS_PORTABILITY_H
#define _SYS_PORTABILITY_H

#include <stdbool.h>
#include <stdint.h>

typedef void *i2c_lowlevel_context;

i2c_lowlevel_context i2c_ll_init(uint8_t i2c_address, uint32_t i2c_speed, uint32_t i2c_timeout_ms,
                                 i2c_lowlevel_config *config);
bool i2c_ll_deinit(i2c_lowlevel_context ctx);
bool i2c_ll_write(i2c_lowlevel_context ctx, uint8_t *data, uint8_t length);
bool i2c_ll_write_reg(i2c_lowlevel_context ctx, uint8_t reg, uint8_t *data, uint8_t length);
bool i2c_ll_read(i2c_lowlevel_context ctx, uint8_t *data, uint8_t length);
bool i2c_ll_read_reg(i2c_lowlevel_context ctx, uint8_t reg, uint8_t *data, uint8_t length);

/* Delay */
#if defined(ESP_PLATFORM)
   #define sys_delay_us(x) ets_delay_us(x)
#elif defined(__linux__)
   #define sys_delay_us(x) usleep(x)
#endif

#endif /* _SYS_PORTABILITY_H */
