CROSS_COMPILE = aarch64-none-linux-gnu-
#CC := $(CROSS_COMPILE)gcc
CC := gcc
CFLAGS ?= -Wall -Wextra -Werror

TARGET := writer
SOURCE := writer.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(TARGET)
