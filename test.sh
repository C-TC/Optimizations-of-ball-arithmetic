#!/bin/bash
## ./test.sh [test number]
## Example Usage
## ./test.sh 3
## See test/README.md for test number definition 

gcc -g -Werror -o base10.out main.c base10.c base10_helper.c -lm
