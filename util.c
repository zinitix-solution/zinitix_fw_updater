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
