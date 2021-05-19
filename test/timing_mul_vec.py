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
            x, cycle1, cycle2, cycle3, cycle4, cycle5 = line.strip().split()
            data_x.append(int(x))
            data_y1.append(float(cycle1))
            data_y2.append(float(cycle2))
            data_y3.append(float(cycle3))
            data_y4.append(float(cycle4))
            data_y5.append(float(cycle5))
    return data_x, data_y1, data_y2, data_y3, data_y4, data_y5

if __name__ == '__main__':
    data_x, data_y1, data_y2, data_y3, data_y4, data_y5 = load_data('timing_mul_vec.data')

    plt.style.use('seaborn-darkgrid')
    plt.figure(figsize=(12.8, 7.2))
    line_mul1, = plt.plot(data_x, data_y1, marker='.', color='black', linewidth=1.5, alpha=0.7, label='mul_fix_precision')
    line_mul2, = plt.plot(data_x, data_y2, marker='*', color='darkblue', linewidth=1.5, alpha=0.7, label='mul_fix_precision_1x_unfold')
    line_mul3, = plt.plot(data_x, data_y3, marker='^', color='darkred', linewidth=1.5, alpha=0.7, label='mul_fix_precision_2x_unfold')
    line_mul4, = plt.plot(data_x, data_y4, marker='s', color='darkorange', linewidth=1.5, alpha=0.7, label='mul_fix_precision_4x_unfold')
    line_mul5, = plt.plot(data_x, data_y5, marker='8', color='darkred', linewidth=1.5, alpha=0.7, label='mul_fix_precision_8x_unfold')
    plt.legend(handles=[line_mul1, line_mul2, line_mul3, line_mul4, line_mul5], fontsize='x-large')

    plt.xscale("log")
    plt.yscale("log")
    plt.xticks(data_x, [r'$2^{' + str(i) + r'}$' for i in range(3, 16)], fontsize=14)
    plt.yticks(fontsize=14)

    plt.title("Intel® Core™ i5-7360U CPU @ 2.30GHz\nL1: 32KB, L2: 256KB, L3: 4MB\nCompiler: clang 12.0.0\n Flag:-march=native -O3", loc='left', fontsize=16, fontweight=1, color='black')
    # plt.title("Intel® Core™ i7-9700K CPU @ 3.60GHz\nL1: 32KB, L2: 256KB, L3: 12MB\nCompiler: gcc 7.5.0", loc='left', fontsize=16, fontweight=1, color='black')
    plt.xlabel("Input Size", fontsize=16)
    plt.ylabel("Runtime [Cycle]", fontsize=16)
    plt.savefig('timing_mul_vec.pdf')

    # plt.show()
