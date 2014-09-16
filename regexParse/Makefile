TOP_DIR = ..
INC_DIR += -I$(TOP_DIR)/include/
OUTPUT_DIR = $(TOP_DIR)/Lib

include $(TOP_DIR)/rules.mk

LIB = libregex.a
target = $(LIB)

AR = ar
CFLAGS += -c

$(target): main.cpp
	$(CC) $(CFLAGS) $(INC_DIR) main.cpp -o main.o;  
	$(AR) -rcs $(OUTPUT_DIR)/$@ main.o;

clean:
	rm -f $(OUTPUT_DIR)/$(target) *.o 

.PHONY: clean
