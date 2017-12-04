#! /bin/bash
    git clone https://github.com/linux-test-project/lcov.git
    lcov --directory ./ --capture --output-file index.info --rc lcov_branch_coverage=1
    genhtml -o index.html index.info --branch-coverage
