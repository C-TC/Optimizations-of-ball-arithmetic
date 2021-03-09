from random import randint
from random import random

if __name__ == '__main__':
    TEST_CASE = 10000
    MAX_DIGITS = 1000
    MAX_SCALAR = 99999999
    TEST_FILE_IN = 'test3.input'
    TEST_FILE_OUT = 'test3.output'
    fi = open(TEST_FILE_IN, 'w')
    fo = open(TEST_FILE_OUT, 'w')
    for _ in range(TEST_CASE):
        digits_number = randint(0, MAX_DIGITS)
        max_number = 10 ** digits_number
        x = randint(-max_number, max_number)
        scalar = randint(-MAX_SCALAR, MAX_SCALAR)
        y = x * scalar
        fi.write("{0} {1}\n".format(x, scalar))
        fo.write("{0}\n".format(y))