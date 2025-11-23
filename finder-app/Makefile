CROSS_COMPILE ?= 
CC = $(CROSS_COMPILE)gcc
FLAGS := -Wall -Werror

# all target
.PHONY: all
all: writer

# A default target which builds the “writer” application
writer: writer.c
	$(CC) $(CFLAGS) writer.c -o writer

# A clean target which removes the “writer” application and all .o files
.PHONY: clean
clean:
	rm -rf writer

# Support for cross-compilation.  
# You should be able to generate an application for the native build platform 
# when GNU make variable CROSS_COMPILE is not specified on the make command line.  
# When CROSS_COMPILE is specified with aarch64-none-linux-gnu- (note the trailing -)
# your makefile should compile successfully using the cross compiler installed in step 1.