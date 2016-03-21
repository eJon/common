CC = g++
CFLAGS = -g -Wall
LIBS = -L/home/w/lib64 -L/usr/local/lib -L/usr/local/lib64 -lnspio_api -lrt -lpthread -luuid
INC = -I/home/w/include

all:
	$(CC) $(CFLAGS) $(INC) $(LIBS) client.cc -o client
	$(CC) $(CFLAGS) $(INC) $(LIBS) server.cc -o server
clean:
	rm *.o -f
	rm client -f && rm server -f
