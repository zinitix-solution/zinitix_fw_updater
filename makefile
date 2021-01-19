OBJECTS = device.o firmware.o main.o util.o
SRCS = device.c firmware.c main.c util.c
CC = gcc
CFLAGS = -g -c
LDFLAGS =  -lm -ludev
TARGET = Zinitix_FWupdate
$(TARGET):$(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)
clean:
	rm -rf $(OBJECTS) $(TARGET) core
device.o : device.c
firmware.o : firmware.c
main.o : main.c
util.o : util.c