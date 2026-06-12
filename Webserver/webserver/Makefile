CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -pthread
TARGET = webserver

SRCS = src/main.c src/server.c src/http.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
