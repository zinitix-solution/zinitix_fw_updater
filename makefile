CC = gcc
CFLAGS=-g -Wall
LDFLAGS=-lm -ludev
OBJS= device.o firmware.o main.o util.o
TARGET=Zinitix_FWupdate

all: $(TARGET)

clean:
	rm -f *.o
	rm -f $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

%.o:%.c $(INC_H_FILES)
	@echo $(CC) -M $(CFLAGS) $(CPPFLAGS) $(DEFINES) $(INCPATH) $<
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(DEFINES) $(INCPATH) $< -o $@

%.o:%.cpp $(INC_H_FILES)
	@echo $(CXX) -M $(CXXFLAGS) $(CPPFLAGS) $(DEFINES) $(INCPATH) $<
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(DEFINES) $(INCPATH) $< -o $@
#device.o : device.c
#	$(CC) -c -o device.o device.c

#firmware.o : firmware.c
#	$(CC) -c -o firmware.o firmware.c

#main.o : main.c
#	$(CC) -c -o main.o main.c

#util.o : util.c
#	$(CC) -c -o util.o util.c

