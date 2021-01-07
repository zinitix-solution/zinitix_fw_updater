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

#include <sys/time.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "util.h"

void Sleep(unsigned long mSec)
{
    usleep(mSec * 1000);
}

int Query_delay(int delay_ms)
{
	struct timeval start_point, end_point;
	double operating_time;

	gettimeofday(&start_point, NULL);

	while(1)
	{
		gettimeofday(&end_point, NULL); 
		operating_time = (double)(end_point.tv_sec)+(double)(end_point.tv_usec)/1000000.0-(double)(start_point.tv_sec)-(double)(start_point.tv_usec)/1000000.0;
		if(operating_time*1000 > delay_ms)
			break;
	}

	return operating_time*1000;
}
