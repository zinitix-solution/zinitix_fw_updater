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
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "common.h"
#include "firmware.h"
#include "device.h"
#include "util.h"

extern int gSetup_Value[DEF_SETUP_LIST_CNT];

u16	fw_ver_major = 0;
u16	fw_ver_minor = 0;
u16	fw_ver_release = 0;
u16	pre_fw_ver_major = 0;
u16	pre_fw_ver_minor = 0;
u16	pre_fw_ver_release = 0;
u16	check_sum = 0;


bool GetFirmwareInfo(void *firmware_bin, vu32 *info_size, vu32 *core_size, vu32 *custom_size, vu32 *regi_size)
{
	bool err = 0;
	fw_binary_info *b;
	if(firmware_bin == NULL)
		return -ENOENT;

	b = (fw_binary_info*)firmware_bin;
	if(info_size != NULL)
		*info_size = (u32)TO_LITTLE_ENDIAN(b->val.info_size[1], b->val.info_size[2], b->val.info_size[3]);
	
	if(core_size != NULL)
		*core_size = (u32)TO_LITTLE_ENDIAN(b->val.core_size[1], b->val.core_size[2], b->val.core_size[3]);

	if(custom_size != NULL)
		*custom_size = (u32)TO_LITTLE_ENDIAN(b->val.custom_size[1], b->val.custom_size[2], b->val.custom_size[3]);

	if(regi_size != NULL)
		*regi_size = (u32)TO_LITTLE_ENDIAN(b->val.register_size[1], b->val.register_size[2], b->val.register_size[3]);
	return err;	
}

bool Firmware_Update_Core( unsigned char* firmware_bin, unsigned char* verify_data, char* err_msg, int UseBootload)
{
	u16 Bootloader_ver;

	u8 cmd_pkg[20];
	int i;
	int flash_addr;
	int page_sz = 1024; 
	int retry_cnt;
	vu32 info_size;
	vu32 core_size;

	if(gSetup_Value[PRODUCT_ID] == 0x650E)
		page_sz = 128;

	GetFirmwareInfo(firmware_bin, &info_size, &core_size, NULL, NULL);
	
	if(UseBootload != 0)
	{
		write_register(F0_FWUPGRADE_MODE, 1); // __rom_nojump_main
		Query_delay(50);
		Bootloader_ver = read_register(0x0013);
		if(Bootloader_ver != 0xBD01)
		{
			printf("Bootloader version mismatch(%04X)\n", Bootloader_ver);
			return 0;
		}
		
		write_register(F0_FWUPGRADE_MODE, 2); // __rom_nojump_main
		Query_delay(50);
		Bootloader_ver = read_register(0x0013);
		if(Bootloader_ver != 0xBD01)
		{
			printf("Bootloader version mismatch(%04X)\n", Bootloader_ver);
			return 0;
		}

		printf("Bootloader version  : %4X\n\n", Bootloader_ver);
	}
	else
	{
		write_register(F0_FWUPGRADE_MODE, 0); // __rom_nojump_main
		Query_delay(50);
	}

FW_DOWNLOAD_INFO:
	memset(verify_data, 0, FW_BUFF_SIZE);
	write_register(0x10F0, 1); // Vendor cmd enable
	Query_delay(10);

	write_register(0x12F0, 1); // nvm init
	Query_delay(10);
	
	write_register(0x13F0, 1); // nvm wp disable
	Query_delay(10);

// Core area
    /****  Mass Erase ******************/
    write_register(F0_FWUPGRADE_MERASE, 0); // erage flash
    Query_delay(120);
    write_register(F0_FWUPGRADE_INIT, 0); // upgrade init
    Query_delay(10);
    write_register(F0_FWUPGRADE_WRITE_MODE, FWUPGRADE_ONLYWRITE); // upgrade mode
    Query_delay(10);
    /***********************************/
    write_register(F0_FWUPGRADE_START_PAGE, 0x0000); // start page address
    Query_delay(10);
    //curr_time = GetTickCount();
	
    for (flash_addr = 0; flash_addr < info_size + core_size; )
    {
        for (i=0; i<page_sz/TC_SECTOR_SZ; i++)
        {
            printf("\rBootloader Update ");
            fw_write_data(F0_FWUPGRADE_PGM, &firmware_bin[flash_addr], TC_SECTOR_SZ); // Vendor cmd enable
            flash_addr += TC_SECTOR_SZ;
            //Sleep(1);
            
            printf("%d Byte / %d Byte", flash_addr, info_size + core_size);
            fflush(stdout);
        }
		Query_delay(gSetup_Value[WRITE_FW_DELAY]);
    }
    write_register(F0_FWUPGRADE_INIT, 0); // init flash
    Sleep(10);

    printf("\n");

	if(1)
	{
		for (flash_addr = 0; flash_addr < info_size + core_size; )
	    {
			for (i = 0; i < page_sz/TC_SECTOR_SZ; i++)
	        {
	            printf("\rBootloader Verify ");
	            read_vendor_data(F0_FWUPGRADE_READ, (u8*)&verify_data[flash_addr], TC_SECTOR_SZ);
				flash_addr += TC_SECTOR_SZ;

	            printf("%d Byte / %d Byte", flash_addr, info_size + core_size);
	            fflush(stdout);
			}
		}
	    
		/* verify */
		if (memcmp((u8 *)&firmware_bin[0], (u8 *)&verify_data[0], info_size + core_size) == 0)
	    {
	        printf("\nBootloader verify Success!\n\n");
		}
	    else
	    {
			if(20 > retry_cnt)
			{
				write_register(0x10F0, 1); // Vendor cmd enable
				Sleep(1);
				write_register(F0_FWUPGRADE_INIT, 0); // init flash
				Sleep(1);
				retry_cnt++;
		        printf("\nBootloader verify Fail!\n");
		        printf("Firmware download retry(%d/%d)!\n", retry_cnt, 20);
				goto FW_DOWNLOAD_INFO;
			}
			else
			{
				sprintf(err_msg, "%s",  "Bootloader download Fail!\n");
		        printf("\n\n ");
				printf(" %s", err_msg);
				return 0;;
			}
	    }
	}

	return 1;

// Info area end
}

bool Firmware_Update(unsigned char* file_path)
{
	FILE* fp = NULL;
	char file_name[255];
	char err_msg[128];
	u8 cmd_pkg[20];
	unsigned char firmware_bin[FW_BUFF_SIZE_650];
	unsigned char verify_data[FW_BUFF_SIZE_650];
	int i;
	int flash_addr;
	int page_sz = 1024; 
	int retry_cnt;

	fw_binary_info *bin_info;
	vu32 info_size;
	vu32 core_size;
	vu32 cust_size;
	vu32 regi_size;
	int flash_start_addr;
	int flash_end_addr;

	int release_ver_address;
	u16 release_ver;
	u16 Bootloader_ver;

	int pass_or_fail;

	u16 u16_lsb, u16_msb;
	u32 info_checksum;
	u32 core_checksum;
	int err = 0;
	int nCurrentMode = 0;

	if(gSetup_Value[PRODUCT_ID] == 0x650E)
		page_sz = 128;

	memset(err_msg, 0, 128);

	retry_cnt = 0;
	
	nCurrentMode = get_mode();

	pre_fw_ver_major = fw_ver_major = read_register(0x0012);
	pre_fw_ver_minor = fw_ver_minor = read_register(0x0121);
	pre_fw_ver_release = fw_ver_release = read_register(0x0013);

	u16_lsb = read_register(0x00E4);
	u16_msb = read_register(0x00E5);
	info_checksum = (u16_msb<<16) | u16_lsb;
	u16_lsb = read_register(0x00E6);
	u16_msb = read_register(0x00E7);
	core_checksum = (u16_msb<<16) | u16_lsb;

    sprintf(file_name, "%s",  file_path);
    printf("\n file name : %s\n", file_name);

    // firmware read
    fp = fopen(file_name, "rb");
    if(fp == NULL)
    {
		sprintf(err_msg, "%s",  "firmware file open fail!\n");
        printf("%s", err_msg);
        goto FW_DOWNLOAD_FAIL;
    }
	
    fseek(fp, 0, SEEK_SET);
    err = fread(firmware_bin, 1, FW_BUFF_SIZE_650, fp);
    fclose(fp);

	bin_info = (fw_binary_info *)firmware_bin;
	GetFirmwareInfo(bin_info, &info_size, &core_size, &cust_size, &regi_size);
	release_ver_address = info_size+core_size+cust_size;
	release_ver = (u16)bin_info->buff16[release_ver_address/2 + 0x13];
	printf("Current firmware version %02X%02X\n", fw_ver_minor, fw_ver_release);
	printf("  New	firmware version %02X%02X\n", bin_info->val.minor_ver, release_ver);
	printf("\n\n");

	if(fw_ver_minor != bin_info->val.minor_ver)
	{
		sprintf(err_msg, "%s",  "Incompatible file!\n");
		printf("%s", err_msg);
		goto FW_DOWNLOAD_FAIL;
	}

	if(bin_info->val.release_ver == 0xB00D)
	{
		if(fw_ver_major != bin_info->val.major_ver)
		{
			if(fw_ver_major <= 9)
			{
				printf("Current FW is Not supported Partial download.\n");
				i = 0;
			}
			else
			{
				printf("Bootloader download : %X -> %X\n", fw_ver_major, bin_info->val.major_ver);
				i = 1;
			}
			if(Firmware_Update_Core(firmware_bin, verify_data, err_msg, i) == 1)
			{
				goto FW_DOWNLOAD_INIT;
			}
			else
			{
		        fp = fopen("vefify_fw.mng.bin", "wb");
		        fwrite(verify_data, 1, FW_BUFF_SIZE_650, fp);
		        fclose(fp);
				goto FW_DOWNLOAD_FAIL;
			}
		}
		else
		{
			if( (fw_ver_major > 0x000A)
			&&(info_checksum != bin_info->val.info_checksum
				|| core_checksum != bin_info->val.core_checksum))
			{
				printf("Bootloader is not same\n");
				printf("Bootloader download : %X -> %X\n", fw_ver_major, bin_info->val.major_ver);
				if(Firmware_Update_Core(firmware_bin, verify_data, err_msg, 0) == 1)
				{
					goto FW_DOWNLOAD_INIT;
				}
				else
				{
					fp = fopen("vefify_fw.mng.bin", "wb");
					fwrite(verify_data, 1, FW_BUFF_SIZE_650, fp);
					fclose(fp);
					goto FW_DOWNLOAD_FAIL;
				}
			}
		}
	}
	else
	{
		printf("New FW is Not supported Partial download.(%s)\n", file_name);
		return 0;
	}

	write_register(F0_FWUPGRADE_MODE, 1); // __rom_nojump_main
	Query_delay(50);

	Bootloader_ver = read_register(0x0013);
	if(Bootloader_ver == 0xBD01)
	{
		write_register(F0_FWUPGRADE_MODE, 2); // __rom_nojump_main
		Query_delay(50);
		Bootloader_ver = read_register(0x0013);
		if(Bootloader_ver != 0xBD01)
		{
			printf("Bootloader version mismatch(%04X)\n", Bootloader_ver);
			return 0;
		}
	}
	else
	{
		printf("Bootloader version mismatch(%04X)\n", Bootloader_ver);
		return 0;
	}

	printf("Bootloader version : %4X\n\n", Bootloader_ver);

FW_DOWNLOAD_INIT:
	flash_start_addr = info_size + core_size;
	flash_end_addr = info_size+core_size+cust_size+regi_size;
	
	memset(verify_data, 0, FW_BUFF_SIZE);
	write_register(0x10F0, 1); // Vendor cmd enable
	Query_delay(10);

	write_register(0x12F0, 1); // nvm init
	Query_delay(10);
	
	write_register(0x13F0, 1); // nvm wp disable
	Query_delay(10);
	write_register(F0_FWUPGRADE_INIT, 0); // init flash

	flash_addr = (info_size + core_size)/page_sz;
	if(gSetup_Value[PRODUCT_ID] == 0xE650)
	{
		/****  Block Erase ******************/ // 1kByte unit
		for(i = flash_addr ; i < 32; i++)
		{
			cmd_pkg[0] = 0;
			cmd_pkg[1] = 0;
			cmd_pkg[2] = i;
			cmd_pkg[3] = 0;
			write_data(F0_FWUPGRADE_BLOCK_ERASE, cmd_pkg, 4); // Vendor cmd enable
			Query_delay(60);
		}
		/****  Sector Erase ******************/ // 32kByte unit
		for(i = 1 ; i < 4; i++)
		{
			cmd_pkg[0] = 1;
			cmd_pkg[1] = 0;
			cmd_pkg[2] = i;
			cmd_pkg[3] = 0;
			write_data(F0_FWUPGRADE_BLOCK_ERASE, cmd_pkg, 4); // Vendor cmd enable
			Query_delay(60);
		}
	}

	write_register(F0_FWUPGRADE_INIT, 0); // init flash
    write_register(F0_FWUPGRADE_START_PAGE, flash_addr); // init flash

    for (flash_addr = flash_start_addr; flash_addr < flash_end_addr; )
    {
        for (i=0; i<page_sz/TC_SECTOR_SZ; i++)
        {
            printf("\rOp_code Update ");
            fw_write_data(F0_FWUPGRADE_PGM, &firmware_bin[flash_addr], TC_SECTOR_SZ); // Vendor cmd enable
            flash_addr += TC_SECTOR_SZ;
            
            printf(" %d Byte / %d Byte", flash_addr, flash_end_addr);
            fflush(stdout);
        }
		Query_delay(gSetup_Value[WRITE_FW_DELAY]);
    }

    write_register(F0_FWUPGRADE_INIT, 0); // init flash
    Sleep(10);

    printf("\n");

	if(gSetup_Value[FW_VERIFY_TYPE] == 0)
	{
		flash_start_addr = (info_size + core_size);
		flash_addr = 0;
		if(fw_ver_major >= 0x0B)
		{
			write_register(F0_FWUPGRADE_START_PAGE, flash_start_addr/page_sz); // init flash
			flash_addr = flash_start_addr;
		}
		for (; flash_addr < flash_end_addr; )
	    {
			for (i = 0; i < page_sz/TC_SECTOR_SZ; i++)
	        {
	            printf("\rOp_code Verify ");
	            read_vendor_data(F0_FWUPGRADE_READ, (u8*)&verify_data[flash_addr], TC_SECTOR_SZ);
				flash_addr += TC_SECTOR_SZ;

	            printf(" %d Byte / %d Byte", flash_addr, flash_end_addr);
	            fflush(stdout);
			}
		}
	    
		/* verify */
		if (memcmp((u8 *)&firmware_bin[flash_start_addr], (u8 *)&verify_data[flash_start_addr], flash_end_addr - flash_start_addr) == 0)
	    {
	        printf("\nOperation Code  verify Success!\n");
		}
	    else
	    {
			if(gSetup_Value[FW_REWRITE_LIMIT] > retry_cnt)
			{
				write_register(0x10F0, 1); // Vendor cmd enable
				Sleep(1);
				write_register(F0_FWUPGRADE_INIT, 0); // init flash
				Sleep(1);
				retry_cnt++;
		        printf("\nOperation Code  verify Fail!\n");
		        printf("\nFirmware download retry(%d/%d)!\n", retry_cnt, gSetup_Value[FW_REWRITE_LIMIT]);
				goto FW_DOWNLOAD_INIT;
			}
			else
			{
				sprintf(err_msg, "%s",  "Firmware verify Fail!\n");
		        printf("\n\n ");
				printf(" %s", err_msg);

		        fp = fopen("vefify_fw.mng.bin", "wb");
		        fwrite(verify_data, 1, FW_BUFF_SIZE_650, fp);
		        fclose(fp);
			}
	    }
	}
	write_register(0x04F0, 0); // ROM Reset
    Sleep(1500);

    read_register(0x10); // TouchPad Mode Enter
    Sleep(10);
	
	fw_ver_major = read_register(0x0012);
	fw_ver_minor = read_register(0x0121);
	fw_ver_release = read_register(0x0013);
	check_sum = read_register(0x012C);

	printf("\n\n");
	printf("Previous firmware version : %02X%02X\n", pre_fw_ver_minor, pre_fw_ver_release);
	printf("Current  firmware version : %02X%02X\n", fw_ver_minor, fw_ver_release);	

	if(fw_ver_major == bin_info->val.major_ver && fw_ver_minor == bin_info->val.minor_ver && fw_ver_release == release_ver
	&& fw_ver_major != 0 && fw_ver_minor != 0 && fw_ver_release != 0)
	{
		pass_or_fail = 1;
	}
	else
	{
        printf("Firmware version read Fail!\n\n");
		pass_or_fail = 0;
	}
	
	if (check_sum != 0x55AA || pass_or_fail == 0)
    {
		if(gSetup_Value[FW_REWRITE_LIMIT] > retry_cnt)
		{
			write_register(0x10F0, 1); // Vendor cmd enable
			Sleep(1);
			write_register(F0_FWUPGRADE_INIT, 0); // init flash
			Sleep(1);
			retry_cnt++;
			if(check_sum != 0x55AA)
			{
				printf("Current  Checksum version : 0x%X\n", check_sum);
		        printf("Firmware checksum Fail!\n\n");
			}
	        printf("Firmware download retry(%d/%d)!\n", retry_cnt, gSetup_Value[FW_REWRITE_LIMIT]);
			goto FW_DOWNLOAD_INIT;
		}
		else
		{
			sprintf(err_msg, "%s",  "Firmware Upgrade Fail!\n");
			printf(" %s", err_msg);
			goto FW_DOWNLOAD_FAIL;
		}
    }

	//printf("Current  Checksum version : 0x%X\n", check_sum);
	printf(ANSI_COLOR_GREEN "Firmware Upgrade Success!\n\n" ANSI_COLOR_RESET);

	set_mode(nCurrentMode);
    	return true;

FW_DOWNLOAD_FAIL:
	if(err_msg[0] != 0)
	{
		fp = fopen("upgrade_fail_log.txt", "w");
		fwrite(err_msg, 1, sizeof(err_msg), fp);
		fclose(fp);
	}
	return false;
}

void initialize_global()
{
	memset(gSetup_Value, 0, sizeof(gSetup_Value));
	gSetup_Value[VENDOR_ID] = 0x14E5;		// JOAN ID
	gSetup_Value[PRODUCT_ID] = 0xE650;		// JOAN ID
	gSetup_Value[WRITE_FW_DELAY] = 15;
}

int update_firmware(unsigned char* bin_file_path)
{
    int nRet = 0;

	//Check File exist!!!
	if( access( (const char *)bin_file_path, F_OK ) != -1 ) {
    // file exists
        printf("Checked the Firmware File");
        //verify firmware bin
    } else {
        // file doesn't exist
        printf("Could not checked the  Firmware file!!!\n");
        //do not firmware update!!!
        return -ENOENT;
    }

	initialize_global();

	if(false == Firmware_Update(bin_file_path))
	{
		nRet = -EPERM;	
	}

	return nRet;
}

int get_version()
{
	char result_buff[256] = "";

	pre_fw_ver_major = read_register(0x0012);
	pre_fw_ver_minor = read_register(0x0121);
	pre_fw_ver_release = read_register(0x0013);

	sprintf(result_buff + strlen(result_buff), "%02X%02X\n", pre_fw_ver_minor, pre_fw_ver_release);
	printf("%s",result_buff);

	return 0;
}

int get_bin_version(char* path)
{
	FILE* fp = NULL;
	fw_binary_info bin_info;
	char result_buff[256] = "";
	short release = 0;
	u32 release_ver_address = 0;
	vu32 info_size; 
	vu32 core_size;
	vu32 custom_size;
	int err = 0;

	// firmware read
    fp = fopen(path, "rb");
    if(fp == NULL)
    {
        return -ENFILE;
    }
    
    fseek(fp, 0, SEEK_SET);
    err = fread(&bin_info, 1, sizeof(fw_binary_info), fp);
    fclose(fp);

	GetFirmwareInfo((unsigned char *)&bin_info, &info_size, &core_size, &custom_size, NULL);
	release_ver_address = info_size + core_size + custom_size;

	pre_fw_ver_major = bin_info.val.major_ver;
	pre_fw_ver_minor = bin_info.val.minor_ver;
	pre_fw_ver_release = bin_info.buff16[release_ver_address/2 + 0x13];
	
	sprintf(result_buff + strlen(result_buff), "%02X%02X\n", pre_fw_ver_minor, pre_fw_ver_release);
	printf("%s",result_buff);
	return 0;
}
