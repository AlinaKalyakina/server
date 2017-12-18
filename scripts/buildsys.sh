#! /bin/bash
OUT=./garbage/
SRC=../src/
INC=../inc/
LCOV=./lcov/bin/
TESTS=../tests/
OUT=./garbage/
    mkdir -p $OUT    
    cd $OUT
    for VAR in maninterface requests srcget testinterface server manager client test statistic statinterface dataop
    do    
    gcc -Wall $SRC/$VAR.c -I $INC -c -o ./$VAR.o --coverage -std=gnu11
    done
    for VAR in server manager client test statistic
    do
    gcc --coverage -g $VAR.o -I $INC statinterface.o maninterface.o requests.o srcget.o testinterface.o dataop.o -o ./$VAR.out  
    done
    cd ..
