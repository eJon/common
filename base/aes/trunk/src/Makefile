BIN = . 

INC = -I./ -I../include/

AD_INC = /home/w/include/
AD_LIB = /home/w/lib64/

LIB_INC = -L./ 

LIBS = 

OBJS = aes_core.o aes.o

TGTS = ../lib/libaes.a

LDFLAGS = -g -shared -fPIC

CFLAGS = -g -Wall -fPIC -DTRACE_TIME

CC = g++

CXXFLAGS = -g -Wall -DLINUX

.PHONY  : all clean 
all: $(TGTS) install

.cc.o:
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

$(TGTS): $(OBJS) 
	$(CC) $(LDFLAGS) -o $(TGTS) $(OBJS) $(INC) $(LIB_INC) $(LIBS)

install:
	cp -f ../include/* $(AD_INC)/aes/.
	cp $(TGTS) $(AD_LIB)

clean:
	$(RM) $(OBJS) $(TGTS)

#Test:$(OBJS)
#	$(CC) $(CFLAGS) $(OBJS) -o GuessAd $(INC) $(LIB_INC) $(LIBS) 
