CC=gcc
CFLAGS=-Wall -g
OBJS_CLIENT=client.o
OBJS_SERVER=server.o
OBJS_EXECPHP=execphp.o
PROGS=server client execphp

ALL: $(PROGS)

server.o: src/server.c lib/server.h
	$(CC) $(CFLAGS) -c $^

client.o: src/client.c lib/client.h
	$(CC) $(CFLAGS) -c $^

execphp.o: src/execphp.c lib/execphp.h
	$(CC) $(CFLAGS) -c $^

server: $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o $@ $(OBJS_SERVER)

client: $(OBJS_CLIENT)
	$(CC) $(CFLAGS) -o $@ $(OBJS_CLIENT)

execphp: $(OBJS_EXECPHP)
	$(CC) $(CLFLAGS) -o $@ $(OBJS_EXECPHP)

clean:
	\rm -rf $(PROGS) $(OBJS_CLIENT) $(OBJS_SERVER) $(OBJS_EXECPHP)
