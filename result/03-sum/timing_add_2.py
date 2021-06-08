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
            x, cycle1, cycle2, cycle3, cycle4, cycle5 = line.strip().split()[:6]
            data_x.append(int(x))
            data_y1.append(float(cycle1) / float(cycle1))
            data_y2.append(float(cycle1) / float(cycle2) * 3)
            data_y3.append(float(cycle1) / float(cycle3) * 7)
            data_y4.append(float(cycle1) / float(cycle4) * 11)
            data_y5.append(float(cycle1) / float(cycle5) * 15)
    return data_x, data_y1, data_y2, data_y3, data_y4, data_y5

if __name__ == '__main__':
    data_x, data_y1, data_y2, data_y3, data_y4, data_y5 = load_data('timing_add_2.data')

    plt.style.use('seaborn-darkgrid')
    plt.figure(figsize=(12.8, 8.5))
    line_mul1, = plt.plot(data_x, data_y1, marker='.', color='black', linewidth=1.5, alpha=0.7, label='add')
    line_mul2, = plt.plot(data_x, data_y2, marker='*', color='darkblue', linewidth=1.5, alpha=0.7, label='sum_4')
    line_mul3, = plt.plot(data_x, data_y3, marker='^', color='darkred', linewidth=1.5, alpha=0.7, label='sum_8')
    line_mul4, = plt.plot(data_x, data_y4, marker='s', color='darkorange', linewidth=1.5, alpha=0.7, label='sum_12')
    line_mul5, = plt.plot(data_x, data_y5, marker='8', color='cyan', linewidth=1.5, alpha=0.7, label='sum_16')
    plt.legend(handles=[line_mul1, line_mul2, line_mul3, line_mul4, line_mul5], fontsize='x-large')

    plt.xscale("log")
    plt.xticks(data_x, [r'$2^{' + str(i+5) + r'}$' for i in range(3, 20)], fontsize=14)
    plt.yticks(fontsize=14)

    plt.title("Intel® Core™ i5-6300HQ CPU @ 2.30GHz (Skylake)\nL1: 32KB, L2: 256KB, L3: 6MB\nCompiler: gcc 7.5.0\nFlag:-march=native -O3", loc='left', fontsize=16, fontweight=1, color='black')
    plt.xlabel("Input Size / Precision [bit]", fontsize=16)
    plt.ylabel("Speedup", fontsize=16)
    plt.savefig('timing_add_2.pdf')

    # plt.show()
