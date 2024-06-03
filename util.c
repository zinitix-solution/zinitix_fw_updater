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

void struprs(char* src)
{
	unsigned int i;
	char temp;
	bool char_area;
	bool char2_area;
	char_area = false;
	char2_area = false;
	i = 0;
	temp = *src;
	while(temp)
	{
		if((temp=='\'')|(temp=='\"'))
		{
			if(!char2_area)
			if(temp=='\'')
			{
				if(char_area)
					char_area = false;
				else
					char_area = true;
			}
			if(!char_area)
			if(temp=='\"')
			{
				if(char2_area)
					char2_area = false;
				else
					char2_area = true;
			}
		}
		if((!char_area)&&(!char2_area))
		{
			if(temp>='a'&&temp<='z')
				temp=temp -'a'+'A';
			*(src+i) = temp;
		}
		i++;
		temp = *(src+i);
	}
	*(src+i) = '\0';
	return;
}


int	char2dec(char *operand) 
{
	int dec = -1;
	int i=0;
	char cValue[128];
	int ntype = -1;

	// 0:binary 	1:decimal	2:hex	3:decimal or error or label
	int mul = 1;
	int nVal = 0;
	int n;
	char ch[3] = { 'B', 'D', 'H' }; 
	
	if((*(operand)=='\'')&&(*(operand+2)=='\''))
	{
		dec = *(operand+1);
		return dec;
	}
	if((*(operand)=='\"')&&(*(operand+3)=='\"'))
	{
		dec = *(operand+1);
		dec<<=8;
		dec|=*(operand+2);
		return dec;
	}
	memset(cValue, 0, sizeof(cValue));
	struprs(operand);
	if(!strcmp(operand, "B"))
		return -1;
	if(!strcmp(operand, "D"))
		return -1;
	if(!strcmp(operand, "H"))
		return -1;
	if(!strcmp(operand, "0X"))
		return -1;
	
	n = (int)strlen(operand);
	if( *(operand+n-1) == '\n' )
		*(operand+n-1) = 0;
	if( *(operand+0) == '0' && *(operand+1) == 'X')
	{
		for(i=2; i<n; i++)	cValue[i-2] = *(operand+i);
		ntype = 2;
	}
	else
	{
		for(i=0; i<3; i++)
		{
			if( *(operand+n-1) == ch[i] )	break; 
		}
		ntype = i;
		if(ntype==2)	// CASE 'H'
		{
			for(i=0; i<n-1; i++)	cValue[i] = *(operand+i);
		}
	}

	switch(ntype)
	{
		case 0: // 'B' binary
			for(i=0; i<n-1; i++)	cValue[i] = *(operand+i);
			dec = 0;
			for(i=(int)(strlen(cValue)-1); i>=0; i--)
			{
				if( cValue[i] == '0' || cValue[i] == '1' )
				{
					nVal = cValue[i] - '0';
					dec += nVal*mul;
					mul *= 2;
					if(dec>0XFFFFFF)
						dec=0XFFFFFF;
				}
				else // ERROR
				{
					dec = -1;
					return -1;
				}
			}
			break;
		case 1: // 'D' decimal
			for(i=0; i<n-1; i++)	cValue[i] = *(operand+i);
			dec = 0;
			for(i=(int)(strlen(cValue)-1); i>=0; i--)
			{
				if( cValue[i] >= '0' && cValue[i] <= '9' )
				{
					nVal = cValue[i] - '0';
					dec += nVal*mul;
					mul *= 10;
					if(dec>0XFFFFFF)
						dec=0XFFFFFF;
				}
				else
				{
					dec = -1;
					return -1;
				}
			}
			break;
		case 2: // 'H' hex
			dec = 0;
			for(i=(int)(strlen(cValue)-1); i>=0; i--)
			{
				if( cValue[i] >= '0' && cValue[i] <= '9' )
				{ 
					nVal = cValue[i] - '0';	
				}
				else if( cValue[i] >= 'A' && cValue[i] <= 'F' )
				{
					nVal = cValue[i] - 'A' + 10;
				}
				else 
				{
					dec = -1;
					return -1;
				}
				
				dec += nVal*mul;
				mul *= 16;
				if(dec>0XFFFFFF)
					dec=0XFFFFFF;
			}
			break;
		case 3: // decimal or error
			for(i=0; i<n; i++)	cValue[i] = *(operand+i);
			dec = 0;
			for(i=(int)(strlen(cValue)-1); i>=0; i--)
			{
				if( cValue[i] >= '0' && cValue[i] <= '9' )
				{
					nVal = cValue[i] - '0';
					dec += nVal*mul;
					mul *= 10;
					if(dec>0XFFFFFF)
						dec=0XFFFFFF;
				}
				else
				{
					dec = -1;
					return -1;
				}
			}
			break;
		default: break;
	}

	return dec;	
}
