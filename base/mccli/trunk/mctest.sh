g++ -c mctest.cc -I/data0/yanyan10/trunk/gtest-1.6.0/include
g++ -c gtest_main.cc -I/data0/yanyan10/trunk/gtest-1.6.0/include
/usr/bin/libtool --mode=link g++ -o mcgtest mctest.o gtest_main.o libmccli.a -L/data0/yanyan10/trunk/gtest-1.6.0/lib -lgtest -lpthread -ldl
./mcgtest
