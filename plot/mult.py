from matplotlib import pyplot as plt

fig = plt.figure()
ax = fig.add_axes([0, 0, 1, 1])
ax.axis('equal')
ball_add = ['memory', 'big integer mult', 'big integer add', 'function call', 'data preparation', 'others']
percents = [0.4124, 0.28, 0.0886, 0.0539, 0.0591, 0.106]
ax.pie(percents, labels=ball_add)
plt.show()