CC = gcc

SERVER = server.c
SER = server
UTIL = util.c

all: server

server: $(SERVER)
	$(CC) $(SERVER) $(UTIL) -o $(SER)

.PHONY: clean
clean:
	rm $(SER)
