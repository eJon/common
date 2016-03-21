CC = g++
CFLAGS = -g -Wall
LIBS = -L/home/w/lib64 -L/usr/local/lib -L/usr/local/lib64 -lnspio_api -lrt -lpthread -luuid
INC = -I/home/w/include

all:
	$(CC) $(CFLAGS) $(INC) $(LIBS) client.cc -o client
	$(CC) $(CFLAGS) $(INC) $(LIBS) front_server.cc -o front_server
	$(CC) $(CFLAGS) $(INC) $(LIBS) uattr_server.cc -o uattr_server
	$(CC) $(CFLAGS) $(INC) $(LIBS) urelation_server.cc -o urelation_server
clean:
	rm *.o -f
	rm client -f && rm front_server -f && rm uattr_server -f && rm urelation_server -f