import matplotlib.pyplot as plt

def load_data(filename):
    data_x = []
    data_y1 = []
    data_y2 = []
    with open(filename, 'r') as fi:
        for line in fi.readlines():
            x, cycle1, cycle2 = line.strip().split(' ')
            data_x.append(int(x))
            data_y1.append(float(cycle1))
            data_y2.append(float(cycle2))
    return data_x, data_y1, data_y2

if __name__ == '__main__':
    data_x, data_y1, data_y2 = load_data('timing_add.data')

    plt.style.use('seaborn-darkgrid')
    plt.figure(figsize=(12.8, 7.2))
    line_add, = plt.plot(data_x, data_y1, marker='.', color='black', linewidth=1.5, alpha=0.7, label='big_integer_add')
    line_precision, = plt.plot(data_x, data_y2, marker='^', color='darkred', linewidth=1.5, alpha=0.7, label='big_integer_add_inplace_fixed_precision')
    plt.legend(handles=[line_add, line_precision], fontsize='x-large')

    plt.xscale("log")
    plt.yscale("log")
    plt.xticks(data_x, [r'$2^{' + str(i) + r'}$' for i in range(3, 13)], fontsize=14)
    plt.yticks(fontsize=14)

    plt.title("Intel® Core™ i5-7360U CPU @ 2.30GHz\nL1: 32KB, L2: 256KB, L3: 4MB\nCompiler: clang 12.0.0", loc='left', fontsize=16, fontweight=1, color='black')
    plt.xlabel("Input Size", fontsize=16)
    plt.ylabel("Runtime [Cycle]", fontsize=16)
    plt.savefig('timing_add.pdf')

    # plt.show()
