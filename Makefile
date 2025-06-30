CC = gcc
CFLAGS = -Wall -pthread
SRCS = monitor_main.c monitor_buffer.c
OBJS = $(SRCS:.c=.o)
EXEC = monitor

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o $(EXEC)
