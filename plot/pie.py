from matplotlib import pyplot as plt

fig = plt.figure()
ax = fig.add_axes([0, 0, 1, 1])
ax.axis('equal')
ball_add = ['floating point alignment', 'memory', 'big integer addition', 'data preparation', 'others']
percents = [0.4459, 0.2396, 0.0959, 0.0307, 0.1879]
ax.pie(percents, labels=ball_add)
plt.show()