INC = -I. -I../../include/ -I../

LIB_INC = -L../../lib/ 

LIBS = -laes

OBJS = test_aes.o
       
TGTS = test_aes

CXXFLAGS = -Wall -DTRACE_TIME -g  -fPIC

CC = g++ -g -fPIC 

.PHONY  : all clean 
all: $(TGTS)

.cc.o:
	$(CC) $(CXXFLAGS) -c $< -o $@ $(INC) 


$(TGTS): $(OBJS) 
	$(CC) $(CXXFLAGS) $(OBJS) -o $(TGTS) $(INC) $(LIB_INC) $(LIBS) 

clean:
	$(RM) $(OBJS) $(TGTS)

