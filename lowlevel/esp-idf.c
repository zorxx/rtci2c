/* \copyright 2024 Zorxx Software. All rights reserved.
 * \license This file is released under the MIT License. See the LICENSE file for details.
 * \brief rtci2c Library esp-idf implementation 
 */
#include <string.h>  /* memcpy */
#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"
#include "rtci2c_private.h"
#include "helpers.h"

typedef struct
{
   rtci2c_lowlevel_config config;
   i2c_master_bus_handle_t bus;
   i2c_master_dev_handle_t device;
} esp_rtci2c_t;

/* ----------------------------------------------------------------------------------------------
 * rtci2c low-level implementation for esp-idf 
 */

bool rtci2c_ll_init(rtci2c_t *ctx, rtci2c_lowlevel_config *config)
{
   i2c_master_bus_config_t bus_cfg = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .i2c_port = config->port,
      .sda_io_num = config->pin_sda,
      .scl_io_num = config->pin_scl,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = true,      
   };
   i2c_device_config_t dev_cfg = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = ctx->i2c_address,
      .scl_speed_hz = ctx->i2c_speed,
   };

   esp_rtci2c_t *l = (esp_rtci2c_t *) calloc(1, sizeof(*l));
   if(NULL == l)
      return false; 
   memcpy(&l->config, config, sizeof(l->config));

   if(i2c_new_master_bus(&bus_cfg, &l->bus) != ESP_OK
   || i2c_master_bus_add_device(l->bus, &dev_cfg, &l->device) != ESP_OK)
   {
      RTCERR("I2C initialization failed");
      free(l);
      return false;
   }

   ctx->lowlevel = l; 
   return true;
}

bool rtci2c_ll_deinit(rtci2c_t *ctx)
{
   esp_rtci2c_t *l = (esp_rtci2c_t *) ctx->lowlevel;
   i2c_del_master_bus(l->bus);
   free(l);
   return true;
}

bool rtci2c_ll_write(rtci2c_t *ctx, uint8_t reg, uint8_t *data, uint8_t length)
{
   esp_rtci2c_t *l = (esp_rtci2c_t *) ctx->lowlevel;
   uint8_t *buffer;
   esp_err_t result;

   if(length > ctx->i2c_max_transfer_length)
      return false;
   buffer = (uint8_t *) malloc(length + 1);
   if(NULL == buffer)
      return false;

   buffer[0] = reg;
   memcpy(&buffer[1], data, length);
   result = i2c_master_transmit(l->device, buffer, length+1, -1);
   free(buffer);

   return (result == ESP_OK);
}

bool rtci2c_ll_read(rtci2c_t *ctx, uint8_t reg, uint8_t *data, uint8_t length)
{
   esp_rtci2c_t *l = (esp_rtci2c_t *) ctx->lowlevel;

   if(length > ctx->i2c_max_transfer_length)
      return false; 

   return (i2c_master_transmit_receive(l->device, &reg, 1, data, length, -1) == ESP_OK);
}