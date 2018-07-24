CC = gcc-8
CFLAGS = -std=gnu99
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
TARGET = base64

.PHONY:default clean

default:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(TARGET) $(OBJS)