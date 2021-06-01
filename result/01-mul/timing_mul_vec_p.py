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
            flops = 5 * int(x) * int(x)
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
    plt.figure(figsize=(12.8, 8.5))
    line_mul1, = plt.plot(data_x, data_y1, marker='.', color='black', linewidth=1.5, alpha=0.7, label='mul_naive')
    line_mul2, = plt.plot(data_x, data_y2, marker='*', color='darkblue', linewidth=1.5, alpha=0.7, label='mul_fix_precision')
    line_mul3, = plt.plot(data_x, data_y3, marker='^', color='darkred', linewidth=1.5, alpha=0.7, label='mul_fix_precision_unroll')
    line_mul4, = plt.plot(data_x, data_y4, marker='s', color='darkorange', linewidth=1.5, alpha=0.7, label='mul_fix_precision_1x_unroll')
    line_mul5, = plt.plot(data_x, data_y5, marker='8', color='cyan', linewidth=1.5, alpha=0.7, label='mul_fix_precision_2x_unroll')
    line_mul6, = plt.plot(data_x, data_y6, marker='v', color='pink', linewidth=1.5, alpha=0.7, label='mul_fix_precision_4x_unroll')
    line_mul7, = plt.plot(data_x, data_y7, marker='2', color='tan', linewidth=1.5, alpha=0.7, label='mul_fix_precision_8x_unroll')
    line_mul8, = plt.plot(data_x, data_y8, marker='X', color='darkviolet', linewidth=1.5, alpha=0.7, label='mul_fix_precision_12x_unroll')
    line_mul9, = plt.plot(data_x, data_y9, marker='H', color='yellow', linewidth=1.5, alpha=0.7, label='mul_fix_precision_16x_unroll')
    line_mul_qua, = plt.plot(8, 1.25, marker='o', color='darkred', label='mul_quad_double') 
    plt.legend(handles=[line_mul1, line_mul2, line_mul3, line_mul4, line_mul5, line_mul6, line_mul7, line_mul8, line_mul9], fontsize='x-large')
    plt.text(8*1.9, 3.0, 'mul_quad_double', color='darkred', fontsize='x-large')
    plt.annotate("",
            xy=(8, 1.25), xycoords='data',
            xytext=(16, 3.0), textcoords='data',
            arrowprops=dict(width=1, headwidth=5, shrink=0.07, facecolor='darkred', edgecolor='darkred', headlength=8),
            fontsize='x-large'
            )
    plt.xscale("log")
    plt.xticks(data_x, [r'$2^{' + str(i+5) + r'}$' for i in range(3, 17)], fontsize=14)
    plt.yticks([i * 0.5 for i in range(28)], fontsize=14)

    plt.axvline(x=2048, linewidth=2, color='orange')
    plt.text(2048*1.05, 13, 'L1 Cache', color='orange', fontsize='x-large')
    plt.axvline(x=16384, linewidth=2, color='orange')
    plt.text(16384*1.05, 13, 'L2 Cache', color='orange', fontsize='x-large')
    plt.axhline(y=12, linewidth=2, color='darkred')
    plt.text(2048*1.05, 12.1, 'Theoretical Maximum Performance', color='darkred', fontsize='x-large')
    # plt.axhline(y=9.6, linewidth=2, color='darkred')
    # plt.text(2048*1.05, 9.7, '80% Theoretical Maximum Performance', color='darkred', fontsize='x-large')

    plt.text((2**12)*1.4, 5.1, "                     16x Speedup                     ", ha="center", va="center", rotation=90, size=15, bbox=dict(boxstyle="darrow,pad=0.3", fc="floralwhite", ec="darkred", lw=2)) 

    plt.title("Intel® Core™ i5-6300HQ CPU @ 2.30GHz (Skylake)\nL1: 32KB, L2: 256KB, L3: 6MB\nCompiler: clang 12.0.0\nFlag:-march=native -O3", loc='left', fontsize=16, fontweight=1, color='black')
    plt.xlabel("Input Size / Precision [bit]", fontsize=16)
    plt.ylabel("Performance [Intops/Cycle]", fontsize=16)
    plt.savefig('timing_mul_vec_p.pdf')

    # speedup
    print(data_y8[9] / data_y1[9]) # 16.89
    print(data_y8[10] / data_y1[10]) # 16.97
    
    # plt.show()
