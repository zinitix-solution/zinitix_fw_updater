// SPDX-License-Identifier: GPL-2.0
/*
 * Zinitix Firmware Updater
 *
 * Copyright (c) 2021 KwangDeok Son <kdson@zinitix.com>
 * Copyright (c) 2021 Zinitix Solution 
 *
 *  The code may be used by anyone for any purpose, 
 * and can firmware update for Zinitix's touchpad.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>
#include <linux/limits.h>
#include <sys/ioctl.h>
#include <dirent.h>

#include "common.h"
#include "device.h"
#include "util.h"

int gSetup_Value[DEF_SETUP_LIST_CNT];

int zntx_open_device(char* path)
{
    int nRet = -1; 
    char devPath[PATH_MAX] = {0,};
    char* openPath = path;

    if(path == NULL)
    {
        if(0 == zntx_find_devpath(devPath))
            openPath = devPath;
        else
            return nRet;
    }
    if((hid_fd = open(openPath, O_WRONLY)) < 0)
    {
        perror("unable to open");
        goto ERROR;
    }
    path = openPath;
    nRet = 0;   //Success!!
    return nRet;
ERROR:
    zntx_close_device();
    return nRet;
}

void zntx_close_device()
{
    close(hid_fd);
}

int write_register(u16 address, u16 value)
{
    u16 buf[32] = { 0,};

    memset(buf, 0, 64);
    buf[0] = 0x0006;
    buf[1] = address;
    buf[2] = value;
    
    return write(hid_fd, buf, 0x12);
}


int write_data(u16 address, u8* value, u16 size)
{
    u16 buf[32] = {0,};
    u8* data_buf = (u8*)buf;
    int res = 0;
    buf[0] = 0x0406;
    buf[1] = address;
    
    memcpy(&data_buf[4], value, size);
    res = write(hid_fd, buf, 18);
    return res; 
}

int fw_write_data(u16 reg, u8* value, u16 size)
{
    u16 buf[32] = {0,};
    u8* data_buf = (u8*)buf;

    buf[0] = 0x1006;
        
    memcpy(&data_buf[2], value, size);
    return write(hid_fd, buf, size + sizeof(u16)*1);
}

u16 read_register(u16 address)
{
	u16 buf[32] = { 0,};
    u16 value = 0;
    int res = 0;
    
    memset(buf, 0x0, sizeof(u16)*32);
    buf[0] = 0x0106;
    buf[1] = address;

    /* Set Feature */
	res = ioctl(hid_fd, HIDIOCSFEATURE(18), buf);
	if (res < 0)
		perror("HIDIOCSFEATURE");

	/* Get Feature */
	memset(buf, 0x0, sizeof(u16)*32);
	buf[0] = 0x0006; /* Report Number */
    res = ioctl(hid_fd, HIDIOCGFEATURE(6), buf);
	if (res < 0)
		perror("HIDIOCGFEATURE");
    else
        value = buf[2];

    return value;
}

u16 read_vendor_data(u16 reg_addr, u8* inbuf, u16 insize    )
{
	u16 nRet = 0;
	u16 buf[32] = { 0,};
	int res = 0;
	buf[0] = 0x0106;
    buf[1] = reg_addr;
    
    memset(inbuf, 0, insize);
	/* Set Feature */
	res = ioctl(hid_fd, HIDIOCSFEATURE(18), buf);
	if (res < 0)
		perror("HIDIOCSFEATURE");

	/* Get Feature */
	memset(buf, 0x0, sizeof(u16)*32);
	buf[0] = 0x0106; /* Report Number */
	res = ioctl(hid_fd, HIDIOCGFEATURE(insize+2), buf);
	if (res < 0)
    {
        perror("HIDIOCGFEATURE");
    }
    else
    {
        memcpy(inbuf, (u8 *)&buf[1], insize);
    }
    
	return nRet;
}

int get_mode()
{
    int nRet = 0;
    int res = 0;
    char buf[64];
    	/* Get Feature */
	memset(buf, 0x0, sizeof(buf));
	buf[0] = 0x04; /* Report Number */
	buf[1] = 0x01; /* Report Number */
	buf[2] = 0x04; /* Report Number */
	buf[3] = 0x00; /* Report Number */
	res = ioctl(hid_fd, HIDIOCGFEATURE(2), buf);
	if (res < 0) {
		perror("HIDIOCGFEATURE");
	} else {
        nRet = (int)buf[1];
	}

    return nRet;
}

int set_mode(int mode)
{
    char buf[64];
    int res = 0;

    /* Set Feature */
	memset(buf, 0x0, sizeof(buf));
	buf[0] = 0x04; /* Report Number */
	buf[1] = (char)mode & 0xFF;
	
	res = ioctl(hid_fd, HIDIOCSFEATURE(2), buf);
	if (res < 0)
		perror("HIDIOCSFEATURE");

    return 0;
}

int zntx_find_devpath(char* deviceFile)
{
    struct dirent * devDirEntry;
	DIR * devDir;
    int nRet = -1;
    
    devDir = opendir("/dev");
	if(!devDir)
	    return -1;

	while((devDirEntry = readdir(devDir)) != NULL)
	{
        if(strstr(devDirEntry->d_name, "hidraw"))
        {
            char rawDevice[PATH_MAX];
            strncpy(rawDevice, devDirEntry->d_name, PATH_MAX);
            snprintf(deviceFile, PATH_MAX, "/dev/%s", devDirEntry->d_name);
            if((hid_fd = open(deviceFile, O_WRONLY)) < 0)
            {
                printf("##unable to open %s\n", deviceFile);
                continue;
            }
            else
            {
                u16 value = read_register(0x0120);
                if(value == 0xE650 || value == 0x650E)
                {
                    nRet = 0;   //Success!!
                    break;
                }
                continue;
            }        
        }
	}
    return nRet;
}