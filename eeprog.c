/***************************************************************************
    copyright            : (C) by 2003-2004 Stefano Barbato
    email                : stefano@codesink.org
    website		 : http://codesink.org/eeprog.html

    $Id: eeprog.c,v 1.28 2004/02/29 11:06:41 tat Exp $
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

int main()
{
    struct lsm303 e;
    int recvData[8]={0}, ret=0, i=0;
/*ret = lsm303_open("/dev/i2c/0", LSM_303_ACCEL_ADDR, &e);
	if(ret < 0)
	{
     printf("Unable to open Accelerometer   device file  \n");
		return;
	}
// Enable Accelerometer
ret = i2c_smbus_write_byte_data(e.fd,CTRL_REG1_A, 0x27);
	if(ret<0)
	{
printf("Error writing data\n");
		return;
	}
	
ret = i2c_smbus_write_byte_data(e.fd, CTRL_REG4_A, 0x28);
	if(ret<0)
	{
printf("Error writing data\n");
		return;
	}
             // Read data from Accelerometer
recvData[0] = i2c_smbus_read_byte_data(e.fd, OUT_X_L_A);
            recvData[1] = i2c_smbus_read_byte_data(e.fd,      OUT_X_H_A);
           recvData[2] =i2c_smbus_read_byte_data(e.fd,  OUT_Y_L_A);
          
recvData[3] = i2c_smbus_read_byte_data(e.fd, OUT_Y_H_A);
recvData[4] = i2c_smbus_read_byte_data(e.fd, OUT_Z_L_A);
recvData[5] = i2c_smbus_read_byte_data(e.fd, OUT_Z_H_A);
for (i = 0 ; i < 6 ; i++)
{
printf("Received Accelerometer data is %d\n",recvData[i]);
	}
lsm303_close(&e);*/
/*************Magnetometer***************************************/
/*ret = lsm303_open("/dev/i2c/0", LSM_303_MAGNET_ADDR, &e);
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
             for (i = 0 ; i < 8 ; i++)
    {
printf("Received Magnetometer data is %d\n",recvData[i]);
    }
lsm303_close(&e);
*/
while(1){
	magread();
	sleep(2);
}
return 0;
}



/*#include <stdio.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "24cXX.h"

#define usage_if(a) do { do_usage_if( a , __LINE__); } while(0);
void do_usage_if(int b, int line)
{
	const static char *eeprog_usage = 
		"I2C-24C08(256 bytes) Read/Write Program, ONLY FOR TEST!\n"
		"Base on 'eeprog' by Stefano Barbato (http://codesink.org/eeprog.html)\n"
		"FriendlyARM Computer Tech. 2009\n";
	if(!b)
		return;
	fprintf(stderr, "%s\n[line %d]\n", eeprog_usage, line);
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


static int read_from_eeprom(struct eeprom *e, int addr, int size)
{
	int ch, i;
	for(i = 0; i < size; ++i, ++addr)
	{
		die_if((ch = eeprom_read_byte(e, addr)) < 0, "read error");
		if( (i % 16) == 0 ) 
			printf("\n %.4x|  ", addr);
		else if( (i % 8) == 0 ) 
			printf("  ");
		printf("%.2x ", ch);
		fflush(stdout);
	}
	fprintf(stderr, "\n\n");
	return 0;
}

static int write_to_eeprom(struct eeprom *e, int addr)
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
		die_if(eeprom_write_byte(e, addr, i), "write error");
	}
	fprintf(stderr, "\n\n");
	return 0;
}

int main(int argc, char** argv)
{
	struct eeprom e;
	int op;

	op = 0;

	usage_if(argc != 2 || argv[1][0] != '-' || argv[1][2] != '\0');
	op = argv[1][1];

	fprintf(stderr, "Open /dev/i2c/0 with 8bit mode\n");
	die_if(eeprom_open("/dev/i2c/0", 0x50, EEPROM_TYPE_8BIT_ADDR, &e) < 0, 
			"unable to open eeprom device file "
			"(check that the file exists and that it's readable)");
	switch(op)
	{
	case 'r':
		fprintf(stderr, "  Reading 256 bytes from 0x0\n");
		read_from_eeprom(&e, 0, 256);
		break;
	case 'w':
		fprintf(stderr, "  Writing 0x00-0xff into 24C08 \n");
		write_to_eeprom(&e, 0);
		break;
	default:
		usage_if(1);
		exit(1);
	}
	eeprom_close(&e);

	return 0;
}
*/
