#ifndef _RTCI2C_H
#define _RTCI2C_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

/* linux */
#if defined(__linux__)
typedef struct
{
   /* Note that it may be necessary to access i2c device files as root */
   const char *device;   /* e.g. "/dev/i2c-0" */
} rtci2c_lowlevel_config;

/* esp-idf */
#elif defined(ESP_PLATFORM)
#include "hal/i2c_types.h"
typedef struct rtci2c_lowlevel_s
{
    i2c_port_t port;
    int pin_sda;
    int pin_scl;
} rtci2c_lowlevel_config;

#else
   #error "Supported OS type not detected"
#endif

typedef void *rtci2c_context;

typedef enum
{
   RTCI2C_DEVICE_DS1307

   /* The following are coming soon
   RTCI2C_DEVICE_DS1302,
   RTCI2C_DEVICE_PCF8563, 
   RTCI2C_DEVICE_DS3231 */
} rtci2c_device_type;

rtci2c_context rtci2c_init(rtci2c_device_type device, uint8_t i2c_address, rtci2c_lowlevel_config *config);
bool rtci2c_deinit(rtci2c_context context);
bool rtci2c_get_datetime(rtci2c_context context, struct tm *datetime);
bool rtci2c_set_datetime(rtci2c_context context, struct tm *datetime);

#endif /* _RTCI2C_H */
