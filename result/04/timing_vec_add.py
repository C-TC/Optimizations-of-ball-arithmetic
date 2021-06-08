import matplotlib.pyplot as plt

if __name__ == '__main__':
    data_x = [8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536]
    data_y1 = [3.31, 3.44, 3.24, 3.47, 3.51, 3.35, 3.31, 3.19, 2.96, 2.48, 2.66, 2.34, 2.45, 2.32]
    data_y2 = [3.42, 3.39, 3.38, 3.45, 3.36, 3.48, 3.15, 3.02, 2.86, 2.67, 2.12, 2.45, 2.09, 1.56]
    data_y3 = [3.56, 3.519, 3.437, 3.659, 3.389, 3.023, 3.152, 3.018, 2.998, 2.653, 2.315, 2.228, 2.018,  1.698 ]
    plt.style.use('seaborn-darkgrid')
    plt.figure(figsize=(12.8, 8.5))
    line_mul1, = plt.plot(data_x, data_y1, marker='.', color='black', linewidth=1.5, alpha=0.7, label='4-element vector')
    line_mul2, = plt.plot(data_x, data_y2, marker='*', color='darkblue', linewidth=1.5, alpha=0.7, label='8-element vector')
    line_mul3, = plt.plot(data_x, data_y3, marker='^', color='darkred', linewidth=1.5, alpha=0.7, label='16-element vector')
    plt.legend(handles=[line_mul1, line_mul2, line_mul3], fontsize='x-large')

    plt.xscale("log")
    plt.xticks(data_x, [r'$2^{' + str(i+5) + r'}$' for i in range(3, 17)], fontsize=14)
    plt.yticks(fontsize=14)

    plt.axvline(x=2048, linewidth=2, color='orange')
    plt.text(2048*1.05, 3.84, 'L1 Cache', color='orange', fontsize='x-large')
    plt.axvline(x=16384, linewidth=2, color='orange')
    plt.text(16384*1.05, 3.84, 'L2 Cache', color='orange', fontsize='x-large')

    plt.title("Intel® Core™ i5-6300HQ CPU @ 2.30GHz (Skylake)\nL1: 32KB, L2: 256KB, L3: 6MB\nCompiler: gcc 7.5.0\nFlag:-march=native -O3", loc='left', fontsize=16, fontweight=1, color='black')
    plt.xlabel("Input Size / Precision [bit]", fontsize=16)
    plt.ylabel("Speedup", fontsize=16)
    plt.savefig('timing_vec_add.pdf')
