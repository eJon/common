CC=gcc
CXX=g++
CXXFLAGS=-g \
  -O2 \
  -Wno-invalid-offsetof \
  -pipe \
  -fPIC \
  -Wall\
  -Wparentheses
  

CFLAGS=
CPPFLAGS=
INCPATH=
DEP_INCPATH=-I../smalltable/output/include \
           -I../smalltable/detail/ \
           -I../smalltable/ \
           -I../3rd/include

EXLIBS=-L../smalltable/output/lib/ -lsmalltable\
 -L../3rd/lib/ -lbsl\
 -lpthread

.PHONY:all
all:adcore

.PHONY:ccpclean
ccpclean:

.PHONY:clean
clean:ccpclean
	rm -rf st_helloworld
	rm -fr adcore
	rm -fr adcore.o
	rm -rf ./output/bin/adcore
	rm -rf ./output/bin/st_helloworld
	rm -rf st_helloworld_loader.o


st_helloworld:st_helloworld_loader.o
	$(CXX) st_helloworld_loader.o $(EXLIBS) -o st_helloworld
	mkdir -p ./output/bin
	cp -f st_helloworld ./output/bin

st_helloworld_loader.o:loader.cpp
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o st_helloworld_loader.o loader.cpp


adcore:adcore.o
	$(CXX) adcore.o $(EXLIBS) -o adcore
	mkdir -p ./output/bin
	cp -f adcore ./output/bin

adcore.o:adcore.cpp
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o adcore.o adcore.cpp




