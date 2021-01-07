#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>
#include <sys/ioctl.h>
#include "common.h"
#include "device.h"
#include "firmware.h"

int main(int argc, char **argv)
{
    unsigned long nArgCmd = 0;
    unsigned long value = 0;
    int err = 0;
	
    //파라미터가 없으면 종료한다.
    if (argc < 2) 
	{
        printf("check parameter!!\n");
        err = -EINVAL;
        goto WORK_END;
    }

    if(0 == strcmp(argv[1], "--get_current_version"))
    {
		printf("Zinitix Touchpad Firmware Version : ");
		nArgCmd = 1;
		argv[2] = "/dev/hidraw0";
    }
    else
    {
    	nArgCmd = strtoul(argv[1], NULL, 10);
    }

    if(nArgCmd == 2)
    {
        err = get_bin_version(argv[2]);
        goto WORK_END;
    }
    
    //Device Open
	if(zntx_open_device(argv[2]) < 0)
	{
		printf(ANSI_COLOR_RED "[ZNTX]Failed Open Device!!" ANSI_COLOR_RESET "\n");
		err = -ENODEV;
        goto WORK_END;
	}

    switch(nArgCmd)
    {
        case 1:
            get_version();
            break;
        case 2:
            //get_bin_version(argv[2]);
            break;
        case 32:
            err = update_firmware(argv[3]);
            break;
        default:
            printf("Please Check Your Parameter[%ld][%s][%s]!!", nArgCmd, argv[1], argv[2]);
            break;    
    }
    zntx_close_device();

WORK_END:
	return err;	
}
