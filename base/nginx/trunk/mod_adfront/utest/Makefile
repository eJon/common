
OBJ=nginx_handler_test

cc_options = -O0 -ftest-coverage -fprofile-arcs

gcov_flag = -lgcov

INC=-I./include/ -I.. -I/home/w/include

LIB_DIR=-L./lib/ -L/home/w/lib64

DEFINE_MACRO=-DUNITTEST_

LIBS=-lpthread  -ldl -lgtest

SRC=../ngx_handler.cc ngx_handler_test.cc

CXX=g++

${OBJ}:${SRC}
		${CXX} -g $(cc_options) $(DEFINE_MACRO)  $(INC) $(SRC) $(LIB_DIR) $(LIBS) -o $(OBJ)

clean:
	rm -rf *.xml *.gcda *.gcno $(OBJ)
