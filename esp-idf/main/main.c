/* \copyright 2024 Zorxx Software. All rights reserved.
 * \license This file is released under the MIT License. See the LICENSE file for details.
 * \brief rtci2c library esp-idf example application
 */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "nvs_flash.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "rtci2c.h"

#define TAG "RTCI2C"

/* The following definitions may change, based on the ESP device,
   RTC device configuration, and wiring between them. */
#define ESP_I2C_PORT I2C_NUM_0
#define ESP_I2C_SDA  GPIO_NUM_21
#define ESP_I2C_SCL  GPIO_NUM_22
#define DEVICE_I2C_ADDRESS 0 /* let the library figure it out */

void app_main(void)
{
   ESP_ERROR_CHECK(nvs_flash_init());
   ESP_ERROR_CHECK(esp_event_loop_create_default() );

   rtci2c_lowlevel_config config;
   config.port = ESP_I2C_PORT;
   config.pin_sda = ESP_I2C_SDA;
   config.pin_scl = ESP_I2C_SCL;
   rtci2c_context *ctx = rtci2c_init(RTCI2C_DEVICE_DS1307, DEVICE_I2C_ADDRESS, &config);
   if(NULL == ctx)
   {
      ESP_LOGE(TAG, "Initialization failed");
   }
   else
   {
      struct tm t;

      for(int i = 0; i < 100; ++i)
      {
         if(!rtci2c_get_datetime(ctx, &t))
         {
            ESP_LOGE(TAG, "Date/tate query failed");
         }
         else
         {
            ESP_LOGI(TAG, "Current: %02u/%02u/%u %u:%02u:%02u",
               t.tm_mday, t.tm_mon, t.tm_year, t.tm_hour, t.tm_min, t.tm_sec);
         }
         vTaskDelay(pdMS_TO_TICKS(100));
      }
      rtci2c_deinit(ctx);
   }

   ESP_LOGI(TAG, "Test application finished");

   for(;;)
      vTaskDelay(portMAX_DELAY);
}
