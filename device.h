#ifndef DEVICE_H
#define DEVICE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int zntx_open_device(char *path);
void zntx_close_device();
int write_register(u16 address, u16 value);
int write_data(u16 reg, u8* value, u16 size);
int fw_write_data(u16 reg, u8* value, u16 size);
u16 read_register(u16 address);
u16 read_vendor_data(u16 reg_addr, u8* inbuf, u16 insize);
int set_mode(int mode);
int get_mode();

#endif