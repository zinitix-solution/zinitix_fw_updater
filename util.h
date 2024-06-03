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

#ifndef UTIL_H
#define UTIL_H

void Sleep(unsigned long mSec);
int Query_delay(int delay_ms);
void struprs(char* src);
int	char2dec(char *operand) ;
#endif