import matplotlib.pyplot as plt

def close_figure(event):
    if event.key == 'escape':
        plt.close(event.canvas.figure)

labels = ['mergesort', 'std::sort', 'mergesort + insertion']
xpoints = [[0.0750148, 0.148477, 0.308785, 0.600822, 1.20165, 2.50532, 5.1538, 10.5441],
           [0.0720158, 0.141594, 0.310394, 0.614296, 1.27585, 2.61412, 5.51629, 11.1187],
           [0.0678809, 0.148913, 0.264966, 0.578551, 1.13413, 2.3418, 4.91364, 10.0002]]

ypoints = [1, 2, 4, 8, 16, 32, 64, 128]
ypoints = list(map(lambda x: x*1000000, ypoints))

for i in range(len(labels)):
    plt.plot(ypoints, xpoints[i], label=labels[i])

plt.title("Sort")
plt.xlabel("$n$")
plt.ylabel("time, s")
plt.legend()
plt.show()