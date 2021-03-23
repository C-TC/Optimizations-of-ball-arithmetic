from random import randint
from random import random

if __name__ == '__main__':
    TEST_CASE = 1
    TEST_FILE_IN = 'test6.input'
    TEST_FILE_OUT = 'test6.output'
    fi = open(TEST_FILE_IN, 'w')
    fo = open(TEST_FILE_OUT, 'w')
    for i in range(5, 20):
        MAX_DIGITS = 1 << i
        print("i={1} MAX_DIGITS={0}".format(MAX_DIGITS, i))
        for _ in range(TEST_CASE):
            max_number = 10 ** MAX_DIGITS
            x = randint(0, max_number)
            y = randint(0, max_number)
            z = x + y
            fi.write("{0} {1}\n".format(x, y))
            fo.write('{0}\n'.format(z))