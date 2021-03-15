from random import randint
from random import random

if __name__ == '__main__':
    TEST_CASE = 1
    MAX_DIGITS = 14
    max_number = 10 ** MAX_DIGITS
    TEST_FILE_IN = 'test5.input'
    TEST_FILE_OUT = 'test5.output'
    fi = open(TEST_FILE_IN, 'w')
    fo = open(TEST_FILE_OUT, 'w')
    for _ in range(TEST_CASE):
        x = randint(-max_number, max_number)
        y = randint(-max_number, max_number)
        z = x / y
        fi.write("{0} {1}\n".format(x, y))
        fo.write("{0:.16f}\n".format(z))
