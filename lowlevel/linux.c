/* \copyright 2024 Zorxx Software. All rights reserved.
 * \license This file is released under the MIT License. See the LICENSE file for details.
 * \brief rtci2c Linux implementation
 */
#include <unistd.h>
#include <stdlib.h>
#include <string.h> /* strdup */
#include <fcntl.h> /* open/close */
#include <pthread.h> /* pthread_mutex */
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "helpers.h"
#include "rtci2c_private.h"

typedef struct linux_rtci2c_s
{
    char *device;
    int handle;
    pthread_mutex_t lock;
} linux_rtci2c_t;

bool rtci2c_ll_init(rtci2c_t *ctx, rtci2c_lowlevel_config *config)
{
   linux_rtci2c_t *l;
   int result = -1;

   l = (linux_rtci2c_t *) malloc(sizeof(*l));
   if(NULL == l)
   {
      RTCERR("[%s] Failed to allocate low-level structure", __func__);
      return -1;
   }

   l->handle = -1;
   l->device = strdup(config->device);
   if(NULL == l->device)
   {
      RTCERR("[%s] Memory allocation error", __func__);
   }
   else
   {
      l->handle = open(l->device, O_RDONLY);
      if(l->handle < 0)
      {
         RTCERR("[%s] Failed to open device '%s'\n", __func__, l->device);
      }
      else if(ioctl(l->handle, I2C_SLAVE, ctx->i2c_address) < 0)
      {
         RTCERR("[%s] Failed to set I2C slave address to 0x%02x\n", __func__, ctx->i2c_address);
      }
      else
         result = 0;
   }

   if(0 == result)
   {
      ctx->lowlevel = l;
      pthread_mutex_init(&l->lock, NULL);
   }
   else
   {
      if(l->handle >= 0)
         close(l->handle);
      free(l);
   }

   RTCERR("[%s] result %d\n", __func__, result);
   return result;
}

bool rtci2c_ll_deinit(rtci2c_t *ctx)
{
   linux_rtci2c_t *l = (linux_rtci2c_t *) ctx->lowlevel; 

   if(NULL == l)
      return 0;

   if(l->handle >= 0)
      close(l->handle);
   pthread_mutex_destroy(&l->lock);
   if(NULL != l->device)
      free(l->device);
   free(l);

   return 0;
}

bool rtci2c_ll_write(rtci2c_t *ctx, uint8_t reg, uint8_t *data, uint8_t length)
{
   linux_rtci2c_t *l = (linux_rtci2c_t *) ctx->lowlevel;
   struct i2c_smbus_ioctl_data args;
   union i2c_smbus_data smdata;
   int result;

   if(length > I2C_SMBUS_BLOCK_MAX)
   {
      RTCERR("[%s] Data length overflow (%u bytes)", __func__, length);
      return false;
   }

   smdata.block[0] = length;
   memcpy(&smdata.block[1], data, length);
   args.read_write = I2C_SMBUS_WRITE;
   args.command = reg;
   args.size = I2C_SMBUS_I2C_BLOCK_DATA;
   args.data = &smdata; 
   result = ioctl(l->handle, I2C_SMBUS, &args);
   if(0 != result)
   {
      RTCERR("[%s] Failed (result %d, errno %d)\n", __func__, result, errno);
      return result;
   }

   return 0; 
}

bool rtci2c_ll_read(rtci2c_t *ctx, uint8_t reg, uint8_t *data, uint8_t length)
{
   linux_rtci2c_t *l = (linux_rtci2c_t *) ctx->lowlevel;
   struct i2c_smbus_ioctl_data args;
   union i2c_smbus_data smdata;
   int result;

   if(length > I2C_SMBUS_BLOCK_MAX)
   {
      RTCERR("[%s] Data length overflow (%u bytes)", __func__, length);
      return false;
   }

   smdata.block[0] = length;
   args.read_write = I2C_SMBUS_READ;
   args.command = reg;
   args.size = I2C_SMBUS_I2C_BLOCK_DATA; 
   args.data = &smdata; 
   result = ioctl(l->handle, I2C_SMBUS, &args);
   if(0 != result)
   {
      RTCERR("[%s] Failed (result %d, errno %d)\n", __func__, result, errno);
      return result;
   }
   memcpy(data, &smdata.block[1], length);

   return 0; 
}