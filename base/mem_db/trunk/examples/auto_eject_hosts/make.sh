#!/bin/bash

rm -rf ./auto_eject_hosts
export LD_LIBRARY_PATH="`pwd`/../../build/debug64/lib/"
g++ auto_eject_hosts.cc -I ../../build/debug64/sdk/mem_db_sdk-0.1.0-debug64/include/ -lmem_db -L ../../build/debug64/lib/ -o auto_eject_hosts


sleep 1;
./auto_eject_hosts
