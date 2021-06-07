import matplotlib.pyplot as plt

def load_data(filename):
    data_x = []
    data_y1 = []
    data_y2 = []
    data_y3 = []
    data_y4 = []
    data_y5 = []
    data_y6 = []
    with open(filename, 'r') as fi:
        for line in fi.readlines():
            x, cycle1, cycle2, cycle3, cycle4, cycle5, cycle6 = line.strip().split()[:7]
            data_x.append(int(x))
            data_y1.append(float(cycle1))
            data_y2.append(float(cycle2))
            data_y3.append(float(cycle3))
            data_y4.append(float(cycle4))
            data_y5.append(float(cycle5))
            data_y6.append(float(cycle6))
    return data_x, data_y1, data_y2, data_y3, data_y4, data_y5, data_y6

if __name__ == '__main__':
    data_x, data_y1, data_y2, data_y3, data_y4, data_y5, data_y6 = load_data('qd_mul_perf.data')

    plt.style.use('seaborn-darkgrid')
    plt.figure(figsize=(12.8, 8.5))
    line_mul1, = plt.plot(data_x, data_y1, marker='.', color='black', linewidth=1.5, alpha=0.7, label='mul_naive')
    line_mul2, = plt.plot(data_x, data_y2, marker='*', color='darkblue', linewidth=1.5, alpha=0.7, label='mul_inplace')
    line_mul3, = plt.plot(data_x, data_y3, marker='^', color='darkred', linewidth=1.5, alpha=0.7, label='mul_inplace_vec')
    line_mul4, = plt.plot(data_x, data_y4, marker='s', color='darkorange', linewidth=1.5, alpha=0.7, label='mul_inplace_vec_2x_unroll')
    line_mul5, = plt.plot(data_x, data_y5, marker='8', color='cyan', linewidth=1.5, alpha=0.7, label='mul_inplace_vec_3x_unroll')
    line_mul6, = plt.plot(data_x, data_y6, marker='v', color='darkviolet', linewidth=1.5, alpha=0.7, label='mul_inplace_vec_4x_unroll')
    plt.legend(handles=[line_mul1, line_mul2, line_mul3, line_mul4, line_mul5, line_mul6,], fontsize='x-large')

    plt.xscale("log")
    plt.xticks(data_x, [r'$2^{' + str(i) + r'}$' for i in range(3, 18)], fontsize=14)
    plt.yticks(fontsize=14)
    plt.axvline(x=512, linewidth=2, color='blue')
    plt.text(512*1.05, 3.5, 'L1 Cache', color='blue', fontsize='x-large')
    plt.axvline(x=4096, linewidth=2, color='blue')
    plt.text(4096*1.05, 3.5, 'L2 Cache', color='blue', fontsize='x-large')
    plt.axvline(x=98304, linewidth=2, color='blue')
    plt.text(98304*1.05, 3.5, 'L3 Cache', color='blue', fontsize='x-large')

    plt.axhline(y=4.8, linewidth=2, color='darkred')
    plt.text(512*1.05, 4.9, '60% Theoretical Maximum Performance (no FMA)', color='darkred', fontsize='x-large')

    plt.title("Intel® Core™ i5-6300HQ CPU @ 2.30GHz (Skylake)\nL1: 32KB, L2: 256KB, L3: 6MB\nCompiler: clang 12.0.0\nFlag:-march=native -O3", loc='left', fontsize=16, fontweight=1, color='black')
    plt.xlabel("Quad-double array size", fontsize=16)
    plt.ylabel("Performance [FLOPS/cycle]", fontsize=16)
    plt.savefig('qd_mul_perf.pdf')

    # plt.show()
