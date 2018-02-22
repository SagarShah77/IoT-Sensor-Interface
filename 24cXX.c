/***************************************************************************
    copyright            : (C) by 2003-2004 Stefano Barbato
    email                : stefano@codesink.org

    $Id: 24cXX.c,v 1.5 2004/02/29 11:05:28 tat Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/fs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <sys/stat.h>
#include "24cXX.h"
#define	CTRL_REG1_A			(0x20)
#define	CTRL_REG2_A			(0x21)
#define	CTRL_REG3_A			(0x22)
#define	CTRL_REG4_A			(0x23)
#define	CTRL_REG5_A			(0x24)	
#define	CTRL_REG6_A			(0x25)	
#define OUT_X_L_A                         		(0x28)
#define OUT_X_H_A       			(0x29)
#define OUT_Y_L_A       			(0x2A)
#define OUT_Y_H_A       			(0x2B)
#define OUT_Z_L_A       			(0x2C)
#define OUT_Z_H_A       			(0x2D)
#define CRA_REG_M 			(0x00)
#define CRB_REG_M			(0x01)
#define MR_REG_M			(0x02)
#define OUT_X_H_M			(0x03)
#define OUT_X_L_M			(0x04)
#define OUT_Z_H_M			(0x05)
#define OUT_Z_L_M			(0x06)
#define OUT_Y_H_M			(0x07)
#define OUT_Y_L_M			(0x08)
#define SR_REG_M 			(0x09)
#define TEMP_OUT_H_M			(0x31)
#define TEMP_OUT_L_M			(0x32)
#define LSM_303_ACCEL_ADDR		(0x19)
#define LSM_303_MAGNET_ADDR 	(0x1E)
#define usage_if(a) do { do_usage_if( a , __LINE__); } while(0);
static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, 
          int size, union i2c_smbus_data *data)
{
 	struct i2c_smbus_ioctl_data args;


args.read_write = read_write;
args.command = command;
args.size = size;
args.data = data;
return ioctl(file,I2C_SMBUS,&args);
}
static inline __s32 i2c_smbus_write_quick(int file, __u8 value)
{	return i2c_smbus_access(file,value,0,I2C_SMBUS_QUICK,NULL);
}
static inline __s32 i2c_smbus_read_byte(int file)
{
  	union i2c_smbus_data data;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,0,I2C_SMBUS_BYTE,&data))
return -1;
	else
		return 0x0FF & data.byte;
}
static inline __s32 i2c_smbus_write_byte(int file, __u8 value)
{
	return i2c_smbus_access(file,I2C_SMBUS_WRITE,value,
	                        I2C_SMBUS_BYTE,NULL);
}

static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command)
{
	union i2c_smbus_data data;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
	                     I2C_SMBUS_BYTE_DATA,&data))
		return -1;
	else
		return 0x0FF & data.byte;
}

static inline __s32 i2c_smbus_write_byte_data(int file, __u8 command, 
                                              __u8 value)
{
union i2c_smbus_data data;
	data.byte = value;
	return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
	                        I2C_SMBUS_BYTE_DATA, &data);
}
static inline __s32 i2c_smbus_read_word_data(int file, __u8 command)
{
	union i2c_smbus_data data;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
	                     I2C_SMBUS_WORD_DATA,&data))
		return -1;
	else
		return 0x0FFFF & data.word;
}

static inline __s32 i2c_smbus_write_word_data(int file, __u8 command, 
                                              __u16 value)
{
	union i2c_smbus_data data;
	data.word = value;
	return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
	                        I2C_SMBUS_WORD_DATA, &data);
}

static inline __s32 i2c_smbus_process_call(int file, __u8 command, __u16 value)
{
	union i2c_smbus_data data;
	data.word = value;
	if (i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
	                     I2C_SMBUS_PROC_CALL,&data))
		return -1;
	else
		return 0x0FFFF & data.word;
}
/* Returns the number of read bytes */
static inline __s32 i2c_smbus_read_block_data(int file, __u8 command, 
                                              __u8 *values)
{
	union i2c_smbus_data data;
	int i;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
	                     I2C_SMBUS_BLOCK_DATA,&data))
return -1;
	else {
		for (i = 1; i <= data.block[0]; i++)
			values[i-1] = data.block[i];
		return data.block[0];
	}
}

static inline __s32 i2c_smbus_write_block_data(int file, __u8 command, 
                                               __u8 length, __u8 *values)
{
 	union i2c_smbus_data data;
	int i;
	if (length > 32)
		length = 32;
	for (i = 1; i <= length; i++)
		data.block[i] = values[i-1];
	data.block[0] = length;
	return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
 I2C_SMBUS_BLOCK_DATA, &data);
}
#define CHECK_I2C_FUNC( var, label ) \
	do { 	if(0 == (var & label)) { \
		fprintf(stderr, "\nError: " \
			#label " function is required. Program halted.\n\n"); \
exit(1); } \
	} while(0);
int lsm303_open(char *dev_fqn, int addr, struct lsm303* e)
{
	int funcs, fd, r;
	e->fd = e->addr = 0;
	e->dev = 0;
	
	fd = open(dev_fqn, O_RDWR);
	if(fd <= 0)
	{
fprintf(stderr, "Error lsm303_open: %s\n", strerror(errno));
		return -1;
	}
// get funcs list
	if((r = ioctl(fd, I2C_FUNCS, &funcs) < 0))
	{
fprintf(stderr, "Error ioctl I2C_FUNCS: %s\n", strerror(errno));
		return -1;
	}
// check for req funcs
CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE );
CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE );
CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE_DATA );
CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE_DATA );
CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_WORD_DATA );
CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_WORD_DATA );
// set working device
	if( ( r = ioctl(fd, I2C_SLAVE, addr)) < 0)
	{
fprintf(stderr, "Error eeprom_open: %s\n", strerror(errno));
		return -1;
	}
	e->fd = fd;
 	e->addr = addr;
 	e->dev = dev_fqn;
	return 0;
}
int lsm303_close(struct lsm303 *e)
{
close(e->fd);
e->fd = -1;
e->dev = 0;
	return 0;
}

#if 0
int eeprom_24c32_write_byte(struct lsm303 *e, __u16 mem_addr, __u8 data)
{
	__u8 buf[3] = { (mem_addr >> 8) & 0x00ff, mem_addr & 0x00ff, data };
	return i2c_write_3b(e, buf);
}
int eeprom_24c32_read_current_byte(struct lsm303* e)
{
	ioctl(e->fd, BLKFLSBUF); // clear kernel read buffer
	return i2c_smbus_read_byte(e->fd);
}
int eeprom_24c32_read_byte(struct lsm303* e, __u16 mem_addr)
{
	int r;
	ioctl(e->fd, BLKFLSBUF); // clear kernel read buffer
	__u8 buf[2] = { (mem_addr >> 8) & 0x0ff, mem_addr & 0x0ff };
	r = i2c_write_2b(e, buf);
	if (r < 0)
		return r;
	r = i2c_smbus_read_byte(e->fd);
	return r;
}
#endif

#if 0
int lsm303_read_current_byte(struct lsm303* e)
{
	ioctl(e->fd, BLKFLSBUF); // clear kernel read buffer
	return i2c_smbus_read_byte(e->fd);
}

int lsm303_read_byte(struct lsm303* e, __u16 mem_addr)
{
	int r;
	ioctl(e->fd, BLKFLSBUF); // clear kernel read buffer
	if(e->type == EEPROM_TYPE_8BIT_ADDR)
	{
		__u8 buf =  mem_addr & 0x0ff;
		r = i2c_write_1b(e, buf);
	} else if(e->type == EEPROM_TYPE_16BIT_ADDR) {
		__u8 buf[2] = { (mem_addr >> 8) & 0x0ff, mem_addr & 0x0ff };
		r = i2c_write_2b(e, buf);
	} else {
		fprintf(stderr, "ERR: unknown eeprom type\n");
		return -1;
	}
	if (r < 0)
		return r;
	r = i2c_smbus_read_byte(e->fd);
	return r;
}
int lsm303_write_byte(struct lsm303 *e, __u16 mem_addr, __u8 data)
{
	if(e->type == EEPROM_TYPE_8BIT_ADDR) {
		__u8 buf[2] = { mem_addr & 0x00ff, data };
		return i2c_write_2b(e, buf);
	} else if(e->type == EEPROM_TYPE_16BIT_ADDR) {
		__u8 buf[3] = 
			{ (mem_addr >> 8) & 0x00ff, mem_addr & 0x00ff, data };
		return i2c_write_3b(e, buf);
	} 
	fprintf(stderr, "ERR: unknown eeprom type\n");
	return -1;
}

#endif

void do_usage_if(int b, int line)
{
	const static char *lsm303_usage = 
		"I2C-LSM303 Program, ONLY FOR TEST!\n";

	if(!b)
		return;

	fprintf(stderr, "%s\n[line %d]\n", lsm303_usage, line);
	exit(1);
}
#define die_if(a, msg) do { do_die_if( a , msg, __LINE__); } while(0);

void do_die_if(int b, char* msg, int line)
{
	if(!b)
		return;
	fprintf(stderr, "Error at line %d: %s\n", line, msg);
	fprintf(stderr, "	sysmsg: %s\n", strerror(errno));
	exit(1);
}
#if 0
static int read_from_lsm303(struct lsm303 *e, int addr, int size)
{
	int ch, i;
	for(i = 0; i < size; ++i, ++addr)
	{
		die_if((ch = lsm303_read_byte(e, addr)) < 0, "read error");
		if( (i % 16) == 0 ) 
		{
			printf("\n %.4x|  ", addr);
		}
		else if( (i % 8) == 0 )
		{
			printf("  ");
		}
		
		printf("%.2x ", ch);
		fflush(stdout);
	}
	
	fprintf(stderr, "\n\n");
	return 0;
}
static int write_to_lsm303(struct lsm303 *e, int addr)
{
	int i;
	for(i=0, addr=0; i<256; i++, addr++)
	{
		if( (i % 16) == 0 ) 
			printf("\n %.4x|  ", addr);
		else if( (i % 8) == 0 ) 
			printf("  ");
		printf("%.2x ", i);
		fflush(stdout);
		die_if(lsm303_write_byte(e, addr, i), "write error");
	}
	fprintf(stderr, "\n\n");
	return 0;
}

#endif
/*******************************************************************************
 Main entry point
 ******************************************************************************/

int magread()
{
    struct lsm303 e;
    int  ret=0, i=0;
    int recvData[8]={0}, xaxis=0,yaxis=0,zaxis=0,temper=0;
    /***********************Magnetometer***************************************/
ret = lsm303_open("/dev/i2c/0", LSM_303_MAGNET_ADDR, &e);
    if(ret < 0)
    {
        printf("Unable to open Magnetometer device file \n");
        return;
    }
ret = i2c_smbus_write_byte_data(e.fd, CRA_REG_M, 0x94);
    if(ret<0)
    {
        printf("Error writing data %d\n",__LINE__);
        return;
    }
    
    ret = i2c_smbus_write_byte_data(e.fd, MR_REG_M, 0x00);
    if(ret<0)
    {
        printf("Error writing data %d\n",__LINE__);
        return;
}

    // Read data from Magnetometer
recvData[0] = i2c_smbus_read_byte_data(e.fd, OUT_X_H_M);
recvData[1] = i2c_smbus_read_byte_data(e.fd, OUT_X_L_M);
recvData[2] = i2c_smbus_read_byte_data(e.fd, OUT_Y_H_M);
recvData[3] = i2c_smbus_read_byte_data(e.fd, OUT_Y_L_M);
recvData[4] = i2c_smbus_read_byte_data(e.fd, OUT_Z_H_M);
recvData[5] = i2c_smbus_read_byte_data(e.fd, OUT_Z_L_M);
recvData[6] = i2c_smbus_read_byte_data(e.fd, TEMP_OUT_H_M);
recvData[7] = i2c_smbus_read_byte_data(e.fd, TEMP_OUT_L_M);


xaxis = recvData[0] * 256 + recvData[1];
yaxis = recvData[2] * 256 + recvData[3];
zaxis = recvData[4] * 256 + recvData[5];
temper = (recvData[6] * 256 + recvData[7])/100;  

     printf("x-axis: %d \n y-axis: %d \n z-axis: %d \n Temperature: %d \n",twos(xaxis),twos(yaxis),twos(zaxis),temper);
 	lsm303_close(&e);
 	return 0;
}

int twos(int a){
   int result;
    result = 0xFFFF & a;
    result = (((result&0x8000)?(0<<15):(1<<15))|((result&0x4000)?(0<<14):(1<<14)) 
            |((result&0x2000)?(0<<13):(1<<13))|((result&0x1000)?(0<<12):(1<<12)) 
            |((result&0x0800)?(0<<11):(1<<11))|((result&0x0400)?(0<<10):(1<<10))
            |((result&0x0200)?(0<<9):(1<<9))|((result&0x0100)?(0<<8):(1<<8))
            |((result&0x0080)?(0<<7):(1<<7))|((result&0x0040)?(0<<6):(1<<6))
            |((result&0x0020)?(0<<5):(1<<5))|((result&0x0010)?(0<<4):(1<<4))
            |((result&0x0008)?(0<<3):(1<<3))|((result&0x0004)?(0<<2):(1<<2))
            |((result&0x0002)?(0<<1):(1<<1))|((result&0x0001)?0:1));
    return result=result+1;
}

