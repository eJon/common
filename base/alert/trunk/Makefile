BIN = .

INC = -I./

LIB_INC = -L.

LIBS =
OBJS = alert_socket.o

TGTS = ./lib/libalertsocket.so

LDFLAGS = -g -shared -fPIC 

CFLAGS = -g -Wall -fPIC -DTRACE_TIME -DHAVE_NETINET_IN_H

CC = g++

CXXFLAGS = -g -Wall -DLINUX

.PHONY  : all clean 
all: $(TGTS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

.cc.o:
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

$(TGTS): $(OBJS) 
	$(CC) $(LDFLAGS) -o $(TGTS) $(OBJS) $(INC) $(LIB_INC) $(LIBS)

clean:
	$(RM) $(OBJS) $(TGTS)

#Test:$(OBJS)
#	$(CC) $(CFLAGS) $(OBJS) -o GuessAd $(INC) $(LIB_INC) $(LIBS) 
