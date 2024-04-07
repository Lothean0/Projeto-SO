CC = gcc
CFLAGS = -Wall -g

all: client server

client: client.o 
	$(CC) $(CFLAGS) -o  $@ $^ $(LIBS)

server: server.o mysystem.o
	$(CC) $(CFLAGS) -o  $@ $^ $(LIBS)

clean:
	rm -f *.o client server mysystem
	rm -f fifos/*