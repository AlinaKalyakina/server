#! /bin/bash
OUT=./garbage/
SRC=../src/
INC=../inc/
LCOV=./lcov/bin/
TESTS=../tests/
GEN=./genprog/
	mkdir -p $OUT
    cd $OUT
    gcc $SRC/generator.c -I $INC -o ./generator.out -Wall --coverage -std=gnu11
    mkdir -p $GEN  
    for VAR in 1 2 3 4
    do
    echo test $VAR
	./generator.out $TESTS/gen$VAR $SRC/test.c $GEN/gen$VAR.c 
    done
    echo test 5
    ./generator.out
    echo test 6
    ./generator.out t1.jfh hfdf $GEN/gen4.c
    echo test 7
    ./generator.out $TESTS/gen3 $TESTS/gen2 $GEN/gen5.c
    echo test 8
    ./generator.out $TESTS/gen2 $TESTS/gen3 $GEN/gen5.c
	git clone https://github.com/linux-test-project/lcov.git
    lcov --directory ./generator.gcda --capture --output-file generator.info --rc lcov_branch_coverage=1
    genhtml -o generator.html generator.info --branch-coverage
