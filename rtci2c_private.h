/* \copyright 2024 Zorxx Software. All rights reserved.
 * \license This file is released under the MIT License. See the LICENSE file for details.
 * \brief rtci2c Library internal interface
 */
#ifndef _RTCI2C_PRIVATE_H
#define _RTCI2C_PRIVATE_H

#include <stdbool.h>
#include <stdint.h>
#include "rtci2c.h"

typedef bool (*rtcdevice_init)(void *rtci2c_ctx);
typedef bool (*rtcdevice_deinit)(void *rtci2c_ctx);
typedef bool (*rtcdevice_get_datetime)(void *rtci2c_ctx, rtci2c_datetime *datetime);

typedef struct rtci2c_s
{
    uint8_t i2c_address;
    uint8_t i2c_max_transfer_length;
    uint8_t i2c_timeout;
    uint32_t i2c_speed;

    rtcdevice_init devfn_init;
    rtcdevice_init devfn_deinit;
    rtcdevice_get_datetime devfn_get_datetime;

    void *lowlevel;
} rtci2c_t;

bool rtci2c_ll_init(rtci2c_t *ctx, rtci2c_lowlevel_config *config);
bool rtci2c_ll_deinit(rtci2c_t *ctx);
bool rtci2c_ll_write(rtci2c_t *ctx, uint8_t reg, uint8_t *data, uint8_t length);
bool rtci2c_ll_read(rtci2c_t *ctx, uint8_t reg, uint8_t *data, uint8_t length);

#endif /* _RTCI2C_PRIVATE_H */