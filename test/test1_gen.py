from random import randint
from random import random

if __name__ == '__main__':
    TEST_CASE = 10000
    MAX_DIGITS = 1000
    LEADING_ZERO = 0.1
    MAX_LEADING_ZERO_NUMBER = 100
    TEST_FILE_IN = 'test1.input'
    TEST_FILE_OUT = 'test1.output'
    fi = open(TEST_FILE_IN, 'w')
    fo = open(TEST_FILE_OUT, 'w')
    for _ in range(TEST_CASE):
        digits_number = randint(0, MAX_DIGITS)
        x = randint(0, 10 ** digits_number)
        y = randint(0, 10 ** digits_number)
        z = x + y
        if random() < LEADING_ZERO:
            fi.write("0" * randint(0, MAX_LEADING_ZERO_NUMBER))
        fi.write("{0} ".format(x))
        if random() < LEADING_ZERO:
            fi.write("0" * randint(0, MAX_LEADING_ZERO_NUMBER))
        fi.write("{0}\n".format(y))
        fo.write('{0}\n'.format(z))