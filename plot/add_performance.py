import matplotlib.pyplot as plt

def load_data(filename):
    data_i = []
    data_x = []
    data_y = []
    with open(filename, 'r') as fi:
        for line in fi.readlines():
            i, cycle = line.strip().split(' ')
            data_i.append(int(i))
            data_x.append(2 << int(i))
            flops = (2 << int(i)) / 9 * 4
            data_y.append(flops / float(cycle))
    return data_i, data_x, data_y

if __name__ == '__main__':
    data_i, data_x, data_y = load_data('add_timing.data')

    plt.style.use('seaborn-darkgrid')
    plt.figure(figsize=(12.8, 7.2))
    line_4a, = plt.plot(data_x, data_y, marker='.', color='black', linewidth=1.5, alpha=0.7, label='naive')
    # line_4b, = plt.plot(data_x, data_4b, marker='^', color='darkblue', linewidth=1.5, alpha=0.7, label='optimized')
    # plt.legend(handles=[line_4a, line_4b], fontsize='x-large')
    plt.xscale("log")
    # plt.yscale("log")
    plt.xticks(data_x, ['2^'+str(i) for i in data_i], fontsize=14)
    plt.yticks(fontsize=14)
    # plt.axvline(x=2048, linewidth=2, color='orange')
    # plt.text(2048+200, 1.55, 'L1 Cache', color='orange', fontsize='x-large')
    # plt.axvline(x=16384, linewidth=2, color='orange')
    # plt.text(16384+1600, 1.55, 'L2 Cache', color='orange', fontsize='x-large')
    # plt.axvline(x=262144, linewidth=2, color='orange')
    # plt.text(262144+1600*16, 1.55, 'L3 Cache', color='orange', fontsize='x-large')
    plt.title("Intel® Core™ i5-7360U CPU @ 2.30GHz\nL1: 32KB, L2: 256KB, L3: 4MB\nCompiler: clang 12.0.0", loc='left', fontsize=16, fontweight=1, color='black')
    plt.xlabel("Input Size", fontsize=16)
    plt.ylabel("Performance [Flops/Cycle]", fontsize=16)
    plt.savefig('add_performance.pdf')

    # plt.show()