VPATH = .
CC = gcc
CFLAGS = -g -Wall -fPIC  -std=c99 -D_GNU_SOURCE
LDFLAGS = -shared -fpic
STRIP = strip

INC = -I./ \

LIBS = -L/usr/local/lib/ 

TARGET = libmccli.so

OBJ_LIST = net.o mccli.o  

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

$(TARGET) : $(OBJ_LIST)
	$(AR) $(ARFLAGS) $(TARGET) $(OBJ_LIST)
	$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJ_LIST) $(LIBS) $(INC)
	#$(STRIP) $(TARGET)
	$(AR) $(ARFLAGS) libmccli.a $(OBJ_LIST)
clean:
	$(RM) $(TARGET) $(OBJ_LIST) libmccli.a
