#! /bin/bash
OUT=./garbage/
SRC=../src/
INC=../inc/
LCOV=./lcov/bin/
TESTS=../tests/
TANDT=../src/TandT.c
	mkdir -p $OUT
    cd $OUT
    gcc $SRC/test.c  $SRC/TandT1.c -I $INC -o ./test.out -Wall --coverage -std=gnu11
    gcc $SRC/tester.c $SRC/TandT2.c -I $INC -o ./tester.out -Wall --coverage -std=gnu11
    echo First test
	./tester.out ./test.out <$TESTS/test1; echo 
    echo Second test
    ./tester.out ./test.out <$TESTS/test2; echo
    echo Third test
    ./tester.out ./test.out; echo
	git clone https://github.com/linux-test-project/lcov.git
    for VAR in tester test TandT1 TandT2
    do
    $LCOV/lcov --directory ./$VAR.gcda --capture --output-file $VAR.info --rc lcov_branch_coverage=1
    $LCOV/genhtml -o $VAR.html $VAR.info --branch-coverage
    done
