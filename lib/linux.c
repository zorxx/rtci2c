/*! \copyright 2024 Zorxx Software. All rights reserved.
 *  \license This file is released under the MIT License. See the LICENSE file for details.
 *  \brief Linux portability implementation
 */
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h> /* strdup */
#include <fcntl.h> /* open/close */
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "sys_linux.h"
#include "sys.h"
#include "helpers.h"

typedef struct linux_rtci2c_s
{
    char *device;
    int handle;
    uint32_t timeout;
} linux_i2c_t;

i2c_lowlevel_context i2c_ll_init(uint8_t i2c_address, uint32_t i2c_speed, uint32_t i2c_timeout_ms,
                                 i2c_lowlevel_config *config)
{
   linux_i2c_t *l;
   int result = -1;

   l = (linux_i2c_t *) malloc(sizeof(*l));
   if(NULL == l)
   {
      SERR("[%s] Failed to allocate low-level structure", __func__);
      return NULL;
   }

   l->handle = -1;
   l->timeout = i2c_timeout_ms;
   l->device = strdup(config->device);
   if(NULL == l->device)
   {
      SERR("[%s] Memory allocation error", __func__);
   }
   else
   {
      l->handle = open(l->device, O_RDWR);
      if(l->handle < 0)
      {
         SERR("[%s] Failed to open device '%s'", __func__, l->device);
      }
      else if(ioctl(l->handle, I2C_SLAVE, i2c_address) < 0)
      {
         SERR("[%s] Failed to set I2C slave address to 0x%02x", __func__, i2c_address);
      }
      else
         result = 0;
   }

   if(0 != result)
   {
      if(l->handle >= 0)
         close(l->handle);
      free(l);
      l = NULL;
   }

   SERR("[%s] result %d", __func__, result);
   return (i2c_lowlevel_context) l;
}

bool i2c_ll_deinit(i2c_lowlevel_context ctx)
{
   linux_i2c_t *l = (linux_i2c_t *) ctx;
   if(NULL == l)
      return true;

   if(l->handle >= 0)
      close(l->handle);
   if(NULL != l->device)
      free(l->device);
   free(l);

   return true;
}

bool i2c_ll_write_reg(i2c_lowlevel_context ctx, uint8_t reg, uint8_t *data, uint8_t length)
{
   linux_i2c_t *l = (linux_i2c_t *) ctx;
   struct i2c_smbus_ioctl_data args;
   union i2c_smbus_data smdata;
   int result = -EINVAL;

   if(length > I2C_SMBUS_BLOCK_MAX)
   {
      SERR("[%s] Data length overflow (%u bytes)", __func__, length);
   }
   else
   {
      smdata.block[0] = length;
      memcpy(&smdata.block[1], data, length);
      args.read_write = I2C_SMBUS_WRITE;
      args.command = reg;
      args.size = I2C_SMBUS_I2C_BLOCK_DATA;
      args.data = &smdata; 
      result = ioctl(l->handle, I2C_SMBUS, &args);
      if(0 == result)
      {
         SDBG("[%s] Success (%u bytes)", __func__, length);
         return true;
      }
   }

   SERR("[%s] Failed (result %d, errno %d)", __func__, result, errno);
   return false;
}

bool i2c_ll_write(i2c_lowlevel_context ctx, uint8_t *data, uint8_t length)
{
   linux_i2c_t *l = (linux_i2c_t *) ctx;
   int result = write(l->handle, data, length);
   if(length == result)
   {
      SDBG("[%s] Success (%u bytes)", __func__, length);
      return true;
   }

   SERR("[%s] Failed (result %d, errno %d)", __func__, result, errno);
   return false;
}

bool i2c_ll_read_reg(i2c_lowlevel_context ctx, uint8_t reg, uint8_t *data, uint8_t length)
{
   linux_i2c_t *l = (linux_i2c_t *) ctx;
   struct i2c_smbus_ioctl_data args;
   union i2c_smbus_data smdata;
   int result = -EINVAL;

   if(length > I2C_SMBUS_BLOCK_MAX)
   {
      SERR("[%s] Data length overflow (%u bytes)", __func__, length);
   }
   else
   {
      smdata.block[0] = length;
      args.read_write = I2C_SMBUS_READ;
      args.command = reg;
      args.size = I2C_SMBUS_I2C_BLOCK_DATA; 
      args.data = &smdata; 
      result = ioctl(l->handle, I2C_SMBUS, &args);
      if(0 == result)
      {
         SDBG("[%s] Success (%u bytes)", __func__, length);
         memcpy(data, &smdata.block[1], length);
         return true;
      }
   }

   SERR("[%s] Failed (result %d, errno %d)", __func__, result, errno);
   memset(&smdata.block[1], 0, length);
   return false;
}

bool i2c_ll_read(i2c_lowlevel_context ctx, uint8_t *data, uint8_t length)
{
   linux_i2c_t *l = (linux_i2c_t *) ctx;
   int result = read(l->handle, data, length);
   if(length == result)
   {
      SDBG("[%s] Success (%u bytes)", __func__, length);
      return true; 
   }

   SERR("[%s] Failed (result %d, errno %d)", __func__, result, errno);
   memset(data,  0, length);
   return false;
}