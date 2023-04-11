import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

file = open("out", "r")
T, K, X, M = list(map(float, file.readline().replace('\n', '').split(' ')))

x = np.linspace(0, X, int(M))
Z =[]

for line in file.readlines():
    Z.append(np.array(list(map(float, line.replace('\n', '').split(' ')[ : - 1]))))

fig, ax = plt.subplots()
plt.grid(True)
ax.set_ylim(min(map(min, Z)), max(map(max, Z)))
ax.set_xlabel("x")
ax.set_ylabel("U(x, t)")
line, = ax.plot(x, Z[0])

def animate(i) :
    ax.set_title(f"T = {int(i * T / K * 1000)} ms")
    line.set_ydata(Z[i]) # update the data.
    return line

ani = animation.FuncAnimation(fig, animate, interval = 10, frames = int(K), save_count = 200)

#To save the animation, use e.g.
#
#ani.save("movie.mp4")
#
# or
#
#writer = animation.FFMpegWriter(
#fps = 15, metadata = dict(artist = 'Me'), bitrate = 1800)
#ani.save("movie.mp4", writer = writer)

plt.show()
