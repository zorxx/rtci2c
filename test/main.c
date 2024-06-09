/* \copyright 2024 Zorxx Software. All rights reserved.
 * \license This file is released under the MIT License. See the LICENSE file for details.
 * \brief rtci2c library Linux example application
 */
#include <stdio.h>
#include "helpers.h"
#include "rtci2c.h"

#define DEVICE_I2C_ADDRESS   0 /* let the library figure it out */
#define I2C_BUS              "/dev/i2c-0"

int main(int argc, char *argv[])
{
   rtci2c_context ctx;
   rtci2c_lowlevel_config config = { I2C_BUS };

   ctx = rtci2c_init(RTCI2C_DEVICE_DS1307, DEVICE_I2C_ADDRESS, &config);
   if(NULL == ctx)
   {
      RTCERR("Initialization failed");
   }
   else
   {
      struct tm datetime;
      if(!rtci2c_get_datetime(ctx, &datetime))
      {
         RTCERR("Date/tate query failed");
      }
      rtci2c_deinit(ctx);
   }

   RTCERR("Test application finished");

   return 0;
}