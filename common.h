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

#ifndef COMMON_H
#define COMMON_H

//////////////////////////////
//common
#define APP_MAJOR_VERSION 4
#define APP_MINOR_VERSION 3

//define
typedef unsigned char 	u8;
typedef unsigned short 	u16;
typedef unsigned int 	u32;

typedef volatile char 	vs8;
typedef volatile short 	vs16;
typedef volatile int 	vs32;

typedef volatile unsigned char 	vu8;
typedef volatile unsigned short v16;
typedef volatile unsigned int 	vu32;

//////////////////////////////
//device

#define VENDOR_ID			0
#define PRODUCT_ID			1
#define READ_REG_DELAY 		2
#define READ_DATA_DELAY		3
#define READ_FW_DELAY		4
#define WRITE_REG_DELAY 	5
#define WRITE_DATA_DELAY 	6
#define FW_VERIFY_TYPE		7		// 0:FW_read, 1:checksum only
#define FW_REWRITE_LIMIT	8
#define HW_CAL_AFTER_DOWNLOAD	9
#define WRITE_FW_DELAY		10

//vars
int hid_fd;


//////////////////////////////
//firmware
typedef union _fw_binary_info
{
	u16 buff16[128*1024/2];
	u32 buff32[128*1024/4]; //128*1024byte
	struct _val
	{
		u32 RESERVED0[8];			// 0
		u32 info_checksum;		// 8
		u32 core_checksum;		// 9
		u16 RESERVED1[6];	// 10 unuse
		u16 major_ver;			// 13
		u16 RESERVED2;
		u16 minor_ver;			// 14
		u16 RESERVED3;
		u16 release_ver;		// 15
		u16 RESERVED4[13];
		u8  info_size[4];		// 21
		u8  core_size[4];		// 22
		u8  custom_size[4];		// 23
		u8  register_size[4];	// 24
	}val;
}fw_binary_info;

//////////////////////////////
//main
#define DEF_SETUP_LIST_CNT 12

//////////////////////////////
//util
//Color Print
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

//extern
extern int hid_fd;
/////////////////////////////////////////////////
#endif