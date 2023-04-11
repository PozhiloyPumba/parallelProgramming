import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np
from mpl_toolkits.mplot3d import Axes3D

file = open("out", "r")
T, K, X, M = list(map(float, file.readline().replace('\n', '').split(' ')))

fig, ax = plt.subplots(subplot_kw = {"projection" : "3d" })
ax.set_xlabel('$x$')
ax.set_ylabel('$t$')
ax.set_zlabel('$U(x, t)$')
ax.set_zlim(- 1, 1.5)

x = np.linspace(0, T, int(K))
y = np.linspace(0, X, int(M))
X, Y = np.meshgrid(y, x)
Z =[]

for line in file.readlines():
    Z.append(np.array(list(map(float, line.replace('\n', '').split(' ')[ : - 1]))))

surf = ax.plot_surface(X, Y, np.array(Z), cmap = cm.coolwarm, linewidth = 0, antialiased = False)

fig.colorbar(surf, shrink = 0.5, aspect = 5)

plt.show()
