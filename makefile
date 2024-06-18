program := Zinitix_FWupdate

objects := device.o \
           firmware.o \
           main.o \
           util.o

libraries := stdc++ rt pthread

source_path := ./src
include_path := ./include

CXX ?= g++ # Compiler: GCC C++ Compiler
#CXX ?= arm-none-linux-gnueabi-g++ # Compiler: arm cross compiler

CXXFLAGS = -Wall -ansi -O3 -g
CXXFLAGS += -D__ENABLE_DEBUG__
CXXFLAGS += -D__ENABLE_OUTBUF_DEBUG__
CXXFLAGS += -D__ENABLE_INBUF_DEBUG__
CXXFLAGS += -D__ENABLE_LOG_FILE_DEBUG__
#CXXFLAGS += -D__ENABLE_SYSLOG_DEBUG__

INC_FLAGS += $(addprefix -I, $(include_path))
LDFLAGS += $(addprefix -l, $(libraries))
#LDFLAGS += -static

VPATH = $(include_path)

vpath %.h $(include_path)
vpath %.c $(source_path)
vpath %.cpp $(source_path)

.SUFFIXES: .c .cpp .h

.PHONY: all
all: $(objects)
	$(CXX) $^ $(CXXFLAGS) $(INC_FLAGS) $(LDFLAGS) -o $(program)
	@chmod 777 $(program)
	@rm -rf $^

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) $(INC_FLAGS)

.PHONY: clean
clean:
	@rm -rf $(objects)
	@rm -rf $(program)
