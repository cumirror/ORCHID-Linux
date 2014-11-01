TOP_DIR = ..
MOD_DIR = .
INCLUDE = -I$(TOP_DIR)/include -I$(MOD_DIR)/include
OUTPUT_DIR = $(TOP_DIR)/output

include $(TOP_DIR)/rules.mk

CFLAGS += -g -Wall
CFLAGS += -I$(TOP_DIR)/include -I$(MOD_DIR)/include

LDFLAGS += -L$(TOP_DIR)/Lib
LDFLAGS += -lpcap -lregex

#查找h文件
INCH = $(shell find $(MOD_DIR) -name '*.h')
INCDIR = $(dir $(INCH))
INCLUDE += $(foreach temp,$(INCDIR), -I$(temp)) 

#查找C文件
SRC = $(shell find $(MOD_DIR) -name '*.c')
SRC += $(wildcard *.c)

target_objs = $(patsubst %.c, %.o, $(SRC))
target = test

$(target): $(target_objs)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $(OUTPUT_DIR)/$@ $(LDFLAGS)

clean:
	rm -fr $(target_objs) $(OUTPUT_DIR)/$(target)
