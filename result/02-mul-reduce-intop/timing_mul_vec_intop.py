import matplotlib.pyplot as plt

def load_data(filename):
    data_x = []
    data_y1 = []
    data_y2 = []
    data_y3 = []
    data_y4 = []
    data_y5 = []
    with open(filename, 'r') as fi:
        for line in fi.readlines():
            x, cycle1, cycle2, cycle3, cycle4, cycle5  = line.strip().split()
            data_x.append(int(x))
            data_y1.append(float(cycle1) / float(cycle1))
            data_y2.append(float(cycle1) / float(cycle2))
            data_y3.append(float(cycle1) / float(cycle3))
            data_y4.append(float(cycle1) / float(cycle4))
            data_y5.append(float(cycle1) / float(cycle5))
    return data_x, data_y1, data_y2, data_y3, data_y4, data_y5

if __name__ == '__main__':
    data_x, data_y1, data_y2, data_y3, data_y4, data_y5 = load_data('timing_mul_vec_intop.data')

    plt.style.use('seaborn-darkgrid')
    plt.figure(figsize=(12.8, 8.5))
    line_mul1, = plt.plot(data_x, data_y1, marker='.', color='black', linewidth=1.5, alpha=0.7, label='mul_fix_precision_16x_unroll')
    line_mul2, = plt.plot(data_x, data_y2, marker='*', color='darkblue', linewidth=1.5, alpha=0.7, label='mul_fix_precision_16x_unroll_reduce_intop_1x')
    line_mul3, = plt.plot(data_x, data_y3, marker='^', color='darkred', linewidth=1.5, alpha=0.7, label='mul_fix_precision_16x_unroll_reduce_intop_2x')
    line_mul4, = plt.plot(data_x, data_y4, marker='s', color='darkorange', linewidth=1.5, alpha=0.7, label='mul_fix_precision_16x_unroll_reduce_intop_4x')
    line_mul5, = plt.plot(data_x, data_y5, marker='8', color='cyan', linewidth=1.5, alpha=0.7, label='mul_fix_precision_16x_unroll_reduce_intop_8x')
    plt.legend(handles=[line_mul1, line_mul2, line_mul3, line_mul4, line_mul5], fontsize='x-large')

    plt.xscale("log")
    plt.xticks(data_x, [r'$2^{' + str(i+5) + r'}$' for i in range(3, 17)], fontsize=14)
    plt.yticks(fontsize=14)

    plt.axvline(x=2048, linewidth=2, color='orange')
    plt.text(2048*1.05, 0.84, 'L1 Cache', color='orange', fontsize='x-large')
    plt.axvline(x=16384, linewidth=2, color='orange')
    plt.text(16384*1.05, 0.84, 'L2 Cache', color='orange', fontsize='x-large')

    plt.title("Intel® Core™ i5-6300HQ CPU @ 2.30GHz (Skylake)\nL1: 32KB, L2: 256KB, L3: 6MB\nCompiler: clang 12.0.0\nFlag:-march=native -O3", loc='left', fontsize=16, fontweight=1, color='black')
    plt.xlabel("Input Size / Precision [bit]", fontsize=16)
    plt.ylabel("Speedup", fontsize=16)
    plt.savefig('timing_mul_vec_intop.pdf')

    # plt.show()
