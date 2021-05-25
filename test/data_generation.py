import random
import operator

U_BITS = 32
UINT_MAX_PLUS1 = 1 << U_BITS

def generate_data(min_val, max_val, op, n, num_op, seed=0):
    opxs = [[] for _ in range(num_op)]
    results = []
    random.seed(seed)
    for i in range(n):
        x1 = random.randint(min_val, max_val)
        x2 = random.randint(min_val, max_val)
        z = op(x1, x2)
        opxs[0].append(x1)
        opxs[1].append(x2)
        for j in range(2, num_op):
            x = random.randint(min_val, max_val)
            z = op(z, x)
            opxs[j].append(x)
        results.append(z)
    return opxs, results

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
        val >>= U_BITS
        size += 1
    return sign, size, bits

def save_to_file(f, opxs, results):
    n = len(results)
    for i in range(n):
        for opx in opxs:
            sign, size, bits = convert_to_bigint_format(opx[i])
            f.write(f"{sign}\t{size}\t")
            f.write("\t\t\t".join(bits))
            f.write("\n")
        sign, size, bits = convert_to_bigint_format(results[i])
        f.write(f"{sign}\t{size}\t")
        f.write("\t\t\t".join(bits))
        f.write("\n")
        f.write("\n")
    return

def create_simple_test(filename, op, num_op):
    n = 1
    with open(filename, 'wt') as f:
        for i in range(17):
            maxBits = (1 << i) * U_BITS
            maxAbsVal = 2 ** maxBits
            opxs, results = generate_data(0, maxAbsVal, op, n, num_op)
            save_to_file(f, opxs, results)
            print(i, "done")
    return

if __name__ == "__main__":
    # create_simple_test("data/simple_add.txt", operator.add)
    # create_simple_test("data/simple_sub.txt", operator.sub)
    # create_simple_test("../data/hard_mul_27bit.txt", operator.mul)
    create_simple_test("../data/hard_add_32bit_12.txt", operator.add, 12)