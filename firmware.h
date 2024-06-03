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

#ifndef FIRMWARE_H
#define FIRMWARE_H

#include <stdbool.h>
#define FW_VERIFY_ON		0
#define FW_BUFF_SIZE        (1024*48) // 48K
#define FW_BUFF_SIZE_650    (1024*128) // 128K
#define TC_SECTOR_SZ		(8 * 2)
#define TC_SECTOR_SZ_650	(8 * 2)

#define TO_LITTLE_ENDIAN(n1, n2, n3) (((u32)n1<<16 & 0x00FF0000) | ((u32)n2<<8 & 0x0000FF00) | ((u32)n3 & 0x000000FF))

#define F0_FWUPGRADE_INIT						0x20F0
#define F0_FWUPGRADE_PGM						0x21F0
#define F0_FWUPGRADE_READ						0x22F0
#define F0_FWUPGRADE_PACKET_SIZE				0x23F0
#define F0_FWUPGRADE_REPEAT						0x24F0
#define F0_FWUPGRADE_MODE						0x25F0
#define F0_FWUPGRADE_FLUSH						0x26F0
#define F0_FWUPGRADE_WRITE_MODE					0x27F0
#define F0_FWUPGRADE_MERASE						0x28F0
#define F0_FWUPGRADE_START_PAGE					0x29F0
#define F0_FWUPGRADE_BLOCK_ERASE				0x2AF0
#define F0_FWUPGRADE_BOOT						0x2BF0
#define F0_FWUPGRADE_CHECKSUM					0x2CF0
#define F0_HW_RESET								0x04F0
#define F0_ROM_RESET							0x05F0
#define F0_ROM_RESET_NO_JUMP_MAIN 				0x06F0

enum FWUPGRADE
{
	ENUM_HW_RESET				= 0x8000,
	ENUM_ROM_RESET				= 0x000F,
	ENUM_ROM_RESET_NO_JUMP_MAIN	= 0x8001,
	ENUM_FWUPGRADE_INIT			= 0x01D0,
	ENUM_FWUPGRADE_PGM			= 0x01D1,
	ENUM_FWUPGRADE_READ			= 0x01D2,
	ENUM_FWUPGRADE_PACKET_SIZE	= 0x01D3,
	ENUM_FWUPGRADE_REPEAT		= 0x01D4,
	ENUM_FWUPGRADE_MODE			= 0x01D5,
	ENUM_FWUPGRADE_FLUSH		= 0x01DD,
	ENUM_FWUPGRADE_WRITE_MODE	= 0x01DE,
	ENUM_FWUPGRADE_MERASE		= 0x01DF
};

enum FWUPGRADE_PROGMODE
{
	FWUPGRADE_PROGRAM		= 0,
	FWUPGRADE_ONLYWRITE		= 1,
	FWUPGRADE_WRITEREPEAT	= 2,
};

extern u16	fw_ver_major;
extern u16	fw_ver_minor;
extern u16	fw_ver_release;
extern u16	pre_fw_ver_major;
extern u16	pre_fw_ver_minor;
extern u16	pre_fw_ver_release;
extern u16	check_sum;

bool Firmware_Update_Core(unsigned char* firmware_bin, unsigned char* verify_data, char* err_msg, int UseBootload);
bool Firmware_Update(unsigned char*  file_path);
bool CLM_Firmware_Update_650_BD03(unsigned char* file_path);
void initialize_global();
int update_firmware(int nArgCmd, unsigned char* bin_file_path);
int get_version();
int get_bin_version(char* path);
int Get_vendor_device();
#endif