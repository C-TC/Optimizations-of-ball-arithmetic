#!/bin/bash
## ./test.sh [test number]
## Example Usage
## ./test.sh 3
## See test/README.md for test number definition 

gcc -O3 -g -Werror -o bash10.out base10.c main.c base10_helper.c -lm
