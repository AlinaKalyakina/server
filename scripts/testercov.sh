#! /bin/bash
OUT=./garbage/
SRC=../src/
INC=../inc/
LCOV=./lcov/bin/
TESTS=../tests/
TANDT=../src/TandT.c
	mkdir -p $OUT
    cd $OUT
    gcc $SRC/TandT.c -I $INC -c -o./TandT.o --coverage -std=gnu11
    for VAR in test testlongq tester testerbad1 testerbad2 testerbad3
    do
    gcc $SRC/$VAR.c  TandT.o -I $INC -o ./$VAR.out -Wall --coverage -std=gnu11
    done
    echo First test
	./tester.out ./test.out <$TESTS/test1; echo 
    echo Second test
    ./tester.out ./test.out <$TESTS/test2; echo
    echo Third test
    ./tester.out ./test.out; echo
    echo Fourth test without argument
    ./tester.out; echo
    echo Firth test long question
    ./tester.out ./testlongq.out; echo
    echo Sixth test 
    ./tester.out ./test.out; echo
    echo Test bad que in question request
    ./testerbad1.out ./test.out; echo
    echo Test bad instruction
    ./testerbad2.out ./test.out; echo
    echo Test bad que in check request
    ./testerbad3.out ./test.out; echo
	git clone https://github.com/linux-test-project/lcov.git
    lcov --directory ./ --capture --output-file index.info --rc lcov_branch_coverage=1
    genhtml -o index.html index.info --branch-coverage
