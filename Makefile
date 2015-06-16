OBJS = socket_server.o utils.o
CFLAGS = -Wall -O2

all: socket_server


socket_server: $(OBJS)
	$(CC) $(OBJS) -o socket_server

utils.o: utils.h

.c.o:
	$(CC) $(CFLAGS) -c $*.c

clean:
	rm -f socket_server *.o
