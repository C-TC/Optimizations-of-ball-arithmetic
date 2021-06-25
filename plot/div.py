from matplotlib import pyplot as plt

fig, axs= plt.subplots(1, 2)
# axs[0].axis('equal')
ball_mul = ['memory', 'bigint mult', 'bigint add', 'func call', 'data prep', 'others']
percents = [0.4124, 0.28, 0.0886, 0.0539, 0.0591, 0.106]
axs[0].pie(percents, labels=ball_mul)
# axs[0].set_title("Ball Mul Profiling")
# ball_div = ['memory', 'big integer mult', 'big integer add', 'others']
# percents = [0.5661, 0.1871, 0.2440, 0.0028]
# axs[0].pie(percents, labels=ball_div)
# axs[1].axis('equal')
ball_div = ['memory', 'bigint mul', 'bigint add', 'others']
percents = [0.5661, 0.1871, 0.2440, 0.0028]
axs[1].pie(percents, labels=ball_div)
# axs[1].set_title("Ball Div Profiling")
plt.show()