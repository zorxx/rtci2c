/* \copyright 2024 Zorxx Software. All rights reserved.
 * \license This file is released under the MIT License. See the LICENSE file for details.
 * \brief rtci2c Library helpers
 */
#ifndef _RTCI2C_HELPERS_H
#define _RTCI2C_HELPERS_H

#include <stdio.h>

/* Debug messaging */
#if defined(RTCI2C_DEBUG_ENABLE) && defined(__linux__)
   #define RTCERR(...) fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n")
   #define RTCDBG(...) fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n")

#elif defined(RTCI2C_DEBUG_ENABLE) && defined(ESP_PLATFORM)
   #include "esp_log.h"
   #define RTCERR(...) ESP_LOGE("RTC", __VA_ARGS__)
   #define RTCDBG(...) ESP_LOGI("RTC", __VA_ARGS__)

#else
   #define RTCERR(...)
   #define RTCDBG(...)

#endif

#define RTC_BCD_TO_DEC(bcd) \
   (((((((uint8_t)(bcd)) & 0xF0) >> 4) % 10) * 10) + ((((uint8_t)(bcd)) & 0x0F) % 10))

#endif /* _RTCI2C_HELPERS_H */