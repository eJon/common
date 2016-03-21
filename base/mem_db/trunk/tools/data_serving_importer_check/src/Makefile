CXX = /usr/bin/g++
CXXFLAGS = -Wall -fPIC -g 

ifneq ($(MAKECMDGOALS), release)
    CXXFLAGS += -g
else
    CXXFLAGS += -DRELEASE
endif

BINDIR = ../bin/

INC = -I ../include -I /home/w/include/ -I /usr/local/include/

LDFLAGS = -L /home/w/lib64/ -lmem_db \
					/usr/local/lib/libboost_serialization.a
#					-L /usr/local/lib/ -lboost_serialization

OBJS = *.o
HELP_OBJS = Base64.o

.PHONY: all clean rebuild release

all: DataImportCheck

DataImportCheck: DataImportCheck.o $(HELP_OBJS)
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/$@ DataImportCheck.o $(HELP_OBJS) $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INC)

clean:
	rm -f $(OBJS) $(BINDIR)/*

rebuild: clean all

release: all
	rm -f $(OBJS)
