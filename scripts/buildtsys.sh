#! /bin/bash
OUT=./garbage/
SRC=../src/
INC=../inc/
LCOV=./lcov/bin/
TESTS=../tests/
OUT=./garbage/
    mkdir -p $OUT    
    cd $OUT
    for VAR in maninterface requests srcget testinterface server manager client test
    do    
    gcc -Wall $SRC/$VAR.c -I $INC -c -o ./$VAR.o --coverage -std=gnu11
    done
    for VAR in server manager client test
    do
    gcc --coverage -g $VAR.o -I $INC maninterface.o requests.o srcget.o testinterface.o -o ./$VAR.out
    done
    git clone https://github.com/linux-test-project/lcov.git

