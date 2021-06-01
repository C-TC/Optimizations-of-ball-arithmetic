import matplotlib.pyplot as plt

def load_data(filename):
    data_x = []
    data_y1 = []
    data_y2 = []
    data_y3 = []
    with open(filename, 'r') as fi:
        for line in fi.readlines():
            x, _, _, cycle1, _, _, cycle2, cycle3 = line.strip().split()
            intop = 10 * int(x)
            data_x.append(int(x))
            data_y1.append(intop / float(cycle1))
            data_y2.append(intop / float(cycle2))
            data_y3.append(intop / float(cycle3))
    return data_x, data_y1, data_y2, data_y3

if __name__ == '__main__':
    data_x, data_y1, data_y2, data_y3 = load_data('timing_add_2.data')

    plt.style.use('seaborn-darkgrid')
    plt.figure(figsize=(12.8, 8.5))
    line_mul1, = plt.plot(data_x, data_y1, marker='.', color='black', linewidth=1.5, alpha=0.7, label='sum_8')
    line_mul2, = plt.plot(data_x, data_y2, marker='*', color='darkblue', linewidth=1.5, alpha=0.7, label='sum_8_unroll_1x')
    line_mul3, = plt.plot(data_x, data_y3, marker='^', color='darkred', linewidth=1.5, alpha=0.7, label='sum_8_unroll_2x')
    plt.legend(handles=[line_mul1, line_mul2, line_mul3], fontsize='x-large')

    plt.xscale("log")
    plt.xticks(data_x, [r'$2^{' + str(i+5) + r'}$' for i in range(3, 20)], fontsize=14)
    plt.yticks(fontsize=14)

    plt.axvline(x=512, linewidth=2, color='orange')
    plt.text(512*1.05, 4.35, 'L1 Cache', color='orange', fontsize='x-large')
    plt.axvline(x=4096, linewidth=2, color='orange')
    plt.text(4096*1.05, 4.35, 'L2 Cache', color='orange', fontsize='x-large')
    plt.axvline(x=65536*1.5, linewidth=2, color='orange')
    plt.text(65536*1.5*1.05, 4.35, 'L3 Cache', color='orange', fontsize='x-large')

    plt.text((2**7)*1.4, 3.0, "                 2.7x Speedup                 ", ha="center", va="center", rotation=90, size=15, bbox=dict(boxstyle="darrow,pad=0.3", fc="floralwhite", ec="darkred", lw=2)) 

    plt.title("Intel® Core™ i5-6300HQ CPU @ 2.30GHz (Skylake)\nL1: 32KB, L2: 256KB, L3: 6MB\nCompiler: clang 12.0.0\nFlag:-march=native -O3", loc='left', fontsize=16, fontweight=1, color='black')
    plt.xlabel("Input Size / Precision [bit]", fontsize=16)
    plt.ylabel("Performance [Intop/Cycle]", fontsize=16)
    plt.savefig('timing_add_3.pdf')

    # speedup
    print(data_y2[4] / data_y1[4]) # 16.89
    print(data_y2[5] / data_y1[5]) # 16.97

    # plt.show()
