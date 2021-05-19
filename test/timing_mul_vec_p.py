import matplotlib.pyplot as plt

def load_data(filename):
    data_x = []
    data_y1 = []
    data_y2 = []
    data_y3 = []
    data_y4 = []
    data_y5 = []
    data_y6 = []
    data_y7 = []
    data_y8 = []
    data_y9 = []
    with open(filename, 'r') as fi:
        for line in fi.readlines():
            x, cycle1, cycle2, cycle3, cycle4, cycle5, cycle6, cycle7, cycle8, cycle9 = line.strip().split()
            flops = 2 * int(x) * int(x)
            data_x.append(int(x))
            data_y1.append(flops / float(cycle1))
            data_y2.append(flops / float(cycle2))
            data_y3.append(flops / float(cycle3))
            data_y4.append(flops / float(cycle4))
            data_y5.append(flops / float(cycle5))
            data_y6.append(flops / float(cycle6))
            data_y7.append(flops / float(cycle7))
            data_y8.append(flops / float(cycle8))
            data_y9.append(flops / float(cycle9))
    return data_x, data_y1, data_y2, data_y3, data_y4, data_y5, data_y6, data_y7, data_y8, data_y9

if __name__ == '__main__':
    data_x, data_y1, data_y2, data_y3, data_y4, data_y5, data_y6, data_y7, data_y8, data_y9 = load_data('timing_mul_vec.data')

    plt.style.use('seaborn-darkgrid')
    plt.figure(figsize=(12.8, 8.2))
    line_mul1, = plt.plot(data_x, data_y1, marker='.', color='black', linewidth=1.5, alpha=0.7, label='mul_fix_naive')
    line_mul2, = plt.plot(data_x, data_y2, marker='*', color='darkblue', linewidth=1.5, alpha=0.7, label='mul_fix_precision')
    line_mul3, = plt.plot(data_x, data_y3, marker='^', color='darkred', linewidth=1.5, alpha=0.7, label='mul_fix_precision_unfold')
    line_mul4, = plt.plot(data_x, data_y4, marker='s', color='darkorange', linewidth=1.5, alpha=0.7, label='mul_fix_precision_1x_unfold')
    line_mul5, = plt.plot(data_x, data_y5, marker='8', color='cyan', linewidth=1.5, alpha=0.7, label='mul_fix_precision_2x_unfold')
    line_mul6, = plt.plot(data_x, data_y6, marker='v', color='pink', linewidth=1.5, alpha=0.7, label='mul_fix_precision_4x_unfold')
    line_mul7, = plt.plot(data_x, data_y7, marker='2', color='tan', linewidth=1.5, alpha=0.7, label='mul_fix_precision_8x_unfold')
    line_mul8, = plt.plot(data_x, data_y8, marker='X', color='darkviolet', linewidth=1.5, alpha=0.7, label='mul_fix_precision_12x_unfold')
    line_mul9, = plt.plot(data_x, data_y9, marker='H', color='yellow', linewidth=1.5, alpha=0.7, label='mul_fix_precision_16x_unfold')
    plt.legend(handles=[line_mul1, line_mul2, line_mul3, line_mul4, line_mul5, line_mul6, line_mul7, line_mul8, line_mul9], fontsize='x-large')

    plt.xscale("log")
    # plt.yscale("log")
    plt.xticks(data_x, [r'$2^{' + str(i) + r'}$' for i in range(3, 17)], fontsize=14)
    plt.yticks([i * 0.5 for i in range(10)], fontsize=14)

    plt.axvline(x=2048, linewidth=2, color='orange')
    plt.text(2048*1.05, 4.3, 'L1 Cache', color='orange', fontsize='x-large')
    plt.axvline(x=16384, linewidth=2, color='orange')
    plt.text(16384*1.05, 4.3, 'L2 Cache', color='orange', fontsize='x-large')
    # plt.axvline(x=524288, linewidth=2, color='orange')
    # plt.text(524288*1.05, 4.3, 'L3 Cache', color='orange', fontsize='x-large')

    plt.title("Intel® Core™ i5-7360U CPU @ 2.30GHz\nL1: 32KB, L2: 256KB, L3: 4MB\nCompiler: clang 12.0.0\n Flag:-march=native -O3", loc='left', fontsize=16, fontweight=1, color='black')
    # plt.title("Intel® Core™ i7-9700K CPU @ 3.60GHz\nL1: 32KB, L2: 256KB, L3: 12MB\nCompiler: gcc 7.5.0", loc='left', fontsize=16, fontweight=1, color='black')
    plt.xlabel("Input Size", fontsize=16)
    plt.ylabel("Performance [Flops/Cycle]", fontsize=16)
    plt.savefig('timing_mul_vec_p.pdf')

    # plt.show()
