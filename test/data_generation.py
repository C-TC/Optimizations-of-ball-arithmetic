import random
import operator

UINT_MAX_PLUS1 = 4294967295 + 1

def generate_data(min_val, max_val, op, n, seed=0):
    op1s = []
    op2s = []
    results = []
    random.seed(seed)
    for i in range(n):
        x = random.randint(min_val, max_val)
        y = random.randint(min_val, max_val)
        z = op(x, y)
        op1s.append(x)
        op2s.append(y)
        results.append(z)
    return op1s, op2s, results

def convert_to_bigint_format(val):
    size = 0
    bits = []
    if val == 0:
        sign = 0
    elif val > 0:
        sign  = 1
    else:
        sign = -1
        val *= -1
    while (val > 0):
        bits.append(str(val % UINT_MAX_PLUS1))
        val //= UINT_MAX_PLUS1
        size += 1
    return sign, size, bits

def save_to_file(f, op1s, op2s, results):
    n = len(op1s)
    assert n == len(op2s) and n == len(results)
    for i in range(n):
        sign, size, bits = convert_to_bigint_format(op1s[i])
        f.write(f"{sign}\t{size}\t")
        f.write("\t\t\t".join(bits))
        f.write("\n")
        sign, size, bits = convert_to_bigint_format(op2s[i])
        f.write(f"{sign}\t{size}\t")
        f.write("\t\t\t".join(bits))
        f.write("\n")
        sign, size, bits = convert_to_bigint_format(results[i])
        f.write(f"{sign}\t{size}\t")
        f.write("\t\t\t".join(bits))
        f.write("\n")
        f.write("\n")
    return

def create_simple_test(filename, op):
    n = 1
    with open(filename, 'wt') as f:
        for i in range(5, 11):
            maxBits = 1 << i
            maxAbsVal = 2 ** maxBits
            op1s, op2s, results = generate_data(-maxAbsVal, maxAbsVal, op, n)
            save_to_file(f, op1s, op2s, results)
    return

if __name__ == "__main__":
    create_simple_test("data/simple_add.txt", operator.add)
    create_simple_test("data/simple_sub.txt", operator.sub)
    create_simple_test("data/simple_mul.txt", operator.mul)