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
            x, cycle1, cycle2, cycle3, cycle4, cycle5, cycle6 = line.strip().split()
            data_x.append(int(x))
            data_y1.append(float(cycle1) / float(cycle1))
            data_y2.append(float(cycle1) / float(cycle2))
            data_y3.append(float(cycle1) / float(cycle3))
            data_y4.append(float(cycle1) / float(cycle4))
            data_y5.append(float(cycle1) / float(cycle5))
            data_y6.append(float(cycle1) / float(cycle6))
    return data_x, data_y1, data_y2, data_y3, data_y4, data_y5, data_y6

if __name__ == '__main__':
    data_x, data_y1, data_y2, data_y3, data_y4, data_y5, data_y6 = load_data('big_int_add.data')

    plt.style.use('seaborn-darkgrid')
    plt.figure(figsize=(12.8, 8.5))
    line_mul1, = plt.plot(data_x, data_y1, marker='.', color='black', linewidth=1.5, alpha=0.7, label='baseline')
    line_mul2, = plt.plot(data_x, data_y2, marker='*', color='darkblue', linewidth=1.5, alpha=0.7, label='inline')
    line_mul3, = plt.plot(data_x, data_y3, marker='^', color='darkred', linewidth=1.5, alpha=0.7, label='intrinsics')
    # line_mul4, = plt.plot(data_x, data_y4, marker='s', color='darkorange', linewidth=1.5, alpha=0.7, label='sum_12')
    line_mul5, = plt.plot(data_x, data_y5, marker='^', color='darkorange', linewidth=1.5, alpha=0.7, label='intrinsics_ILP')
    line_mul6, = plt.plot(data_x, data_y6, marker='^', color='cyan', linewidth=1.5, alpha=0.7, label='intrinsics_vectorized')
    plt.legend(handles=[line_mul1, line_mul2, line_mul3, line_mul5, line_mul6], fontsize='x-large')

    plt.xscale("log")
    plt.xticks(data_x, [r'$2^{' + str(i+5) + r'}$' for i in range(3, 20)], fontsize=14)
    plt.yticks(fontsize=14)

    height = max(max(data_y1), max(data_y2), max(data_y3), max(data_y5), max(data_y6)) * 1.05
    plt.axvline(x=1365.3, linewidth=2, color='orange')
    plt.text(1365.3*1.05, height, 'L1 Cache', color='orange', fontsize='x-large')
    plt.axvline(x=10922.7, linewidth=2, color='orange')
    plt.text(10922.7*1.05, height, 'L2 Cache', color='orange', fontsize='x-large')
    plt.axvline(x=262144, linewidth=2, color='orange')
    plt.text(262144*1.05, height, 'L3 Cache', color='orange', fontsize='x-large')

    plt.title("Intel® Core™ i5-6300HQ CPU @ 2.30GHz (Skylake)\nL1: 32KB, L2: 256KB, L3: 6MB\nCompiler: g++ 7.5.0\nFlag:-march=native -O3", loc='left', fontsize=16, fontweight=1, color='black')
    plt.xlabel("Input Size / Precision [x 32 bits]", fontsize=16)
    plt.ylabel("Speedup", fontsize=16)
    plt.savefig('big_int_add_plot.pdf')

    plt.show()