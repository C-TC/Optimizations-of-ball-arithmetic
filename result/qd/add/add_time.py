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
    with open(filename, 'r') as fi:
        for line in fi.readlines():
            x, cycle1, cycle2, cycle3, cycle4, cycle5, cycle6, cycle7, cycle8 = line.strip().split()[:9]
            data_x.append(int(x))
            data_y1.append(float(cycle1) / float(cycle1))
            data_y2.append(float(cycle1) / float(cycle2))
            data_y3.append(float(cycle1) / float(cycle3))
            data_y4.append(float(cycle1) / float(cycle4))
            data_y5.append(float(cycle1) / float(cycle5))
            data_y6.append(float(cycle1) / float(cycle6))
            data_y7.append(float(cycle1) / float(cycle7))
            data_y8.append(float(cycle1) / float(cycle8))
    return data_x, data_y1, data_y2, data_y3, data_y4, data_y5, data_y6, data_y7, data_y8

if __name__ == '__main__':
    data_x, data_y1, data_y2, data_y3, data_y4, data_y5, data_y6, data_y7, data_y8 = load_data('qd_add_speed.data')

    plt.style.use('seaborn-darkgrid')
    plt.figure(figsize=(12.8, 8.5))
    line_add1, = plt.plot(data_x, data_y1, marker='.', color='black', linewidth=1.5, alpha=0.7, label='add_naive')
    line_add2, = plt.plot(data_x, data_y2, marker='*', color='darkblue', linewidth=1.5, alpha=0.7, label='add_inplace')
    line_add3, = plt.plot(data_x, data_y3, marker='^', color='darkred', linewidth=1.5, alpha=0.7, label='add_inplace_vec')
    line_add4, = plt.plot(data_x, data_y4, marker='s', color='darkorange', linewidth=1.5, alpha=0.7, label='add_inplace_vec_2x_unroll')
    line_add5, = plt.plot(data_x, data_y5, marker='8', color='cyan', linewidth=1.5, alpha=0.7, label='add_inplace_vec_3x_unroll')
    line_add6, = plt.plot(data_x, data_y6, marker='v', color='tan', linewidth=1.5, alpha=0.7, label='add_inplace_vec_4x_unroll')
    line_add7, = plt.plot(data_x, data_y7, marker='2', color='darkviolet', linewidth=1.5, alpha=0.7, label='add_inplace_vec_6x_unroll')
    line_add8, = plt.plot(data_x, data_y8, marker='X', color='pink', linewidth=1.5, alpha=0.7, label='add_inplace_vec_8x_unroll')
    plt.legend(handles=[line_add1, line_add2, line_add3, line_add4, line_add5, line_add6, line_add7, line_add8,], fontsize='x-large')

    plt.xscale("log")
    plt.xticks(data_x, [r'$2^{' + str(i) + r'}$' for i in range(3, 18)], fontsize=14)
    plt.yticks(fontsize=14)
    plt.axvline(x=512, linewidth=2, color='blue')
    plt.text(512*1.05, 5, 'L1 Cache', color='blue', fontsize='x-large')
    plt.axvline(x=4096, linewidth=2, color='blue')
    plt.text(4096*1.05, 5, 'L2 Cache', color='blue', fontsize='x-large')
    plt.axvline(x=98304, linewidth=2, color='blue')
    plt.text(98304*1.05, 5, 'L3 Cache', color='blue', fontsize='x-large')

    plt.title("Intel® Core™ i5-6300HQ CPU @ 2.30GHz (Skylake)\nL1: 32KB, L2: 256KB, L3: 6MB\nCompiler: clang 12.0.0\nFlag:-march=native -O3", loc='left', fontsize=16, fontweight=1, color='black')
    plt.xlabel("Quad-double array size", fontsize=16)
    plt.ylabel("Speedup", fontsize=16)
    plt.savefig('qd_add_speed.pdf')

    # plt.show()
