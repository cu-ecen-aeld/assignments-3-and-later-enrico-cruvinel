CROSS_COMPILE ?= 
CC = $(CROSS_COMPILE)gcc
FLAGS := -Wall -Werror

# A default target which builds the “aesdsocket” application
aesdsocket: aesdsocket.c
	$(CC) $(CFLAGS) aesdsocket.c -o aesdsocket

# all target
.PHONY: all
all: aesdsocket

# A clean target which removes the “aesdsocket” application and all .o files
.PHONY: clean
clean:
	rm -rf aesdsocket

# Support for cross-compilation.  
# You should be able to generate an application for the native build platform 
# when GNU make variable CROSS_COMPILE is not specified on the make command line.  
# When CROSS_COMPILE is specified with aarch64-none-linux-gnu- (note the trailing -)
# your makefile should compile successfully using the cross compiler installed in step 1.