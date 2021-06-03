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
            data_y1.append( 4 * int(x) / float(cycle1))
            data_y2.append( 4 * int(x) / float(cycle2))
            data_y3.append( 1 * int(x) / float(cycle3))
            data_y4.append( 1 * int(x) / float(cycle4))
            data_y5.append( 1 * int(x) / float(cycle5))
    return data_x, data_y1, data_y2, data_y3, data_y4, data_y5

if __name__ == '__main__':
    data_x, data_y1, data_y2, data_y3, data_y4, data_y5 = load_data('test/add.data')

    plt.style.use('seaborn-darkgrid')
    plt.figure(figsize=(12.8, 8.5))
    line_mul1, = plt.plot(data_x, data_y1, marker='.', color='black', linewidth=1.5, alpha=0.7, label='add (4n)')
    line_mul2, = plt.plot(data_x, data_y2, marker='*', color='darkblue', linewidth=1.5, alpha=0.7, label='add_inline (4n)')
    line_mul3, = plt.plot(data_x, data_y3, marker='^', color='darkred', linewidth=1.5, alpha=0.7, label='add_inline_unfold_1x (n)')
    line_mul4, = plt.plot(data_x, data_y4, marker='s', color='darkorange', linewidth=1.5, alpha=0.7, label='add_inline_unfold_1x_intrinsics (n)')
    line_mul5, = plt.plot(data_x, data_y5, marker='8', color='cyan', linewidth=1.5, alpha=0.7, label='add_inline_unfold_2x_intrinsics (n)')
    # line_mul6, = plt.plot(data_x, data_y8, marker='v', color='pink', linewidth=1.5, alpha=0.7, label='sum_5_unfold_1x')
    # line_mul7, = plt.plot(data_x, data_y9, marker='2', color='tan', linewidth=1.5, alpha=0.7, label='sum_5_unfold_2x')
    # line_mul8, = plt.plot(data_x, data_y8, marker='X', color='darkviolet', linewidth=1.5, alpha=0.7, label='mul_fix_precision_12x_unfold')
    # line_mul9, = plt.plot(data_x, data_y9, marker='H', color='yellow', linewidth=1.5, alpha=0.7, label='mul_fix_precision_16x_unfold')
    plt.legend(handles=[line_mul1, line_mul2, line_mul3, line_mul4, line_mul5], fontsize=8)

    plt.xscale("log")
    # plt.yscale("log")
    plt.xticks(data_x, [r'$2^{' + str(i) + r'}$' for i in range(3, 23)], fontsize=14)
    plt.yticks(fontsize=14)

    # plt.axvline(x=2048, linewidth=2, color='orange')
    # plt.text(2048*1.05, 0.95, 'L1 Cache', color='orange', fontsize='x-large')
    # plt.axvline(x=16384, linewidth=2, color='orange')
    # plt.text(16384*1.05, 0.95, 'L2 Cache', color='orange', fontsize='x-large')

    fontsize = 8
    height = max(max(data_y1), max(data_y2), max(data_y3),max(data_y4), max(data_y5))
    # plt.axhline(y=12, linewidth=2, color='darkred')
    # plt.text(2048*1.05, 12.1, 'Theoretical Maximum Performance', color='darkred', fontsize=fontsize)
    plt.axvline(x=5461.3, linewidth=2, color='orange')
    plt.text(5461.3*0.7, height*1.07, 'L1 Cache', color='orange', fontsize=fontsize)
    plt.axvline(x=10922.7, linewidth=2, color='orange')
    plt.text(10922.7*0.7, height*1.1, 'L2 Cache', color='orange', fontsize=fontsize)
    plt.axvline(x=262144, linewidth=2, color='orange')
    plt.text(262144 * 0.7, height*1.07, 'L3 Cache', color='orange', fontsize=fontsize)

    plt.title("Intel® Core™ i7-7700HQ CPU @ 2.80GHz (Kabylake)\nL1: 128KB, L2: 256KB, L3: 6MB\nCompiler: gcc 7.5.0\nFlag:-march=native -O3", loc='left', fontsize=16, fontweight=1, color='black')
    # plt.title("Intel® Core™ i7-9700K CPU @ 3.60GHz\nL1: 32KB, L2: 256KB, L3: 12MB\nCompiler: gcc 7.5.0", loc='left', fontsize=16, fontweight=1, color='black')
    plt.xlabel("Input Size", fontsize=16)
    plt.ylabel("IntOps Per Second", fontsize=16)
    plt.savefig('timing_add_opt.pdf')

    plt.show()
