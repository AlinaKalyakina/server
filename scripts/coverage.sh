#! /bin/bash
OUT=./garbage/
SRC=../src/
INC=../inc/
LCOV=./lcov/bin/
TESTS=../tests/
	mkdir -p $OUT
    cd $OUT
    for VAR in tester test
    do
	gcc $SRC/$VAR.c -I $INC -o ./$VAR.out -Wall --coverage -std=gnu11
    done
    echo First test
	./tester.out ./test.out <$TESTS/1; echo 
    echo Second test
    ./tester.out ./test.out <$TESTS/2; echo
    echo Third test
    ./tester.out ./test.out; echo
	git clone https://github.com/linux-test-project/lcov.git
    for VAR in tester test
    do
	$LCOV/lcov --directory ./$VAR.gcda --capture --output-file $VAR.info --rc lcov_branch_coverage=1
    done
	for VAR in tester test
    do
	$LCOV/genhtml -o $VAR.html $VAR.info --branch-coverage
    done
