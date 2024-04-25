CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS = 
DEBUG_FLAGS = -DDEBUG_MODE
all: folders server client

server: bin/server

client: bin/client

folders:
	@mkdir -p src include obj bin tmp 
  
bin/server: obj/server.o obj/mysystem.o obj/exec_pipes.o
	$(CC) $(LDFLAGS) $^ -o $@

bin/client: obj/client.o
	$(CC) $(LDFLAGS) $^ -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

clean:
	rm -rf obj/* tmp/* bin/*
