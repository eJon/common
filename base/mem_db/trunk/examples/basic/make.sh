#!/bin/bash

PROJECT=basic

rm -rf ./$PROJECT
export LD_LIBRARY_PATH="`pwd`/../../build/release64/lib/"
g++ -g ${PROJECT}.cc -I ../../build/release64/sdk/mem_db_sdk-0.1.0-release64/include/ -lmem_db -L ../../build/release64/lib/ -o ${PROJECT}


sleep 1;
./${PROJECT}
