import matplotlib.pyplot as plt

def close_figure(event):
    if event.key == 'escape':
        plt.close(event.canvas.figure)

labels = ['seq_base_mul', 'seq_transpose_mul', 'parallel_base_mul(8)', 'parallel_transpose_mul(8)', 'parallel_block_mul(8)', 'parallel_block_transpose_mul(8)']
xpoints = [[9.359e-06, 3.6807e-05, 0.0357497, 0.312915, 1.30443, 1.95952, 2.86217, 4.0228, 5.41845, 7.15177, 8.73759],
           [8.171e-06, 1.802e-05, 0.099101, 0.813306, 2.80665, 4.48605, 6.69745, 9.42029, 12.9483, 17.1854, 22.4313],
           [0.00058709, 0.00100125, 0.060312, 0.1859, 0.479332, 0.679087, 0.973529, 1.26137, 1.61876, 2.03483, 2.54219],
           [0.00158855, 0.00199908, 0.0371689, 0.174587, 0.636254, 0.757743, 1.08781, 1.50025, 1.97908, 2.72953, 3.36338],
           [0.00201815, 0.00130108, 0.0162401, 0.0890383, 0.231007, 0.437467, 0.537108, 0.754548, 0.958871, 1.28331, 1.58334],
           [0.000982954, 0.00177845, 0.0312695, 0.146145, 0.441206, 0.662784, 0.971329, 1.40628, 1.8133, 2.46198, 2.99801]]

labels_par = ['2 threads', '4 threads', '8 threads']
titles = ['parallel_base_mul', 'parallel_transpose_mul', 'parallel_block_mul', 'parallel_block_transpose_mul']

parallel_mul = [
           [[0.000568232, 0.000952012, 0.108043, 0.424799, 1.13661, 1.6078, 2.21146, 3.05816, 3.91798, 5.09361, 6.21126],
           [0.000535407, 0.000879098, 0.0716612, 0.232268, 0.577972, 0.983511, 1.29165, 1.7364, 2.44558, 2.99715, 3.82558],
           [0.00058709, 0.00100125, 0.060312, 0.1859, 0.479332, 0.679087, 0.973529, 1.26137, 1.61876, 2.03483, 2.54219]],
           [[0.000406968, 0.000482955, 0.0522221, 0.440035, 1.43098, 2.26595, 3.37436, 4.74127, 6.50899, 8.68115, 11.1829],
           [0.000552657, 0.000941047, 0.0603555, 0.251098, 0.745765, 1.28886, 1.86085, 2.62139, 3.54016, 4.60041, 6.38257],
           [0.00158855, 0.00199908, 0.0371689, 0.174587, 0.636254, 0.757743, 1.08781, 1.50025, 1.97908, 2.72953, 3.36338]],
           [[0.00035696, 0.00043239, 0.0321539, 0.118038, 0.335858, 0.642075, 0.88693, 1.23112, 1.78722, 2.65395, 2.97219],
           [0.0004685, 0.000464729, 0.0147761, 0.0884184, 0.293093, 0.363781, 0.625306, 0.778379, 1.02714, 1.33342, 1.68149],
           [0.00201815, 0.00130108, 0.0162401, 0.0890383, 0.231007, 0.437467, 0.537108, 0.754548, 0.958871, 1.28331, 1.58334]],
           [[0.000314778, 0.000436722, 0.0613821, 0.304848, 1.01029, 1.50351, 2.25924, 3.43082, 4.42119, 6.17425, 7.67907],
           [0.000676069, 0.000811074, 0.0355788, 0.200037, 0.613392, 0.913422, 1.35643, 1.85676, 2.48476, 3.32013, 4.20781],
           [0.000982954, 0.00177845, 0.0312695, 0.146145, 0.441206, 0.662784, 0.971329, 1.40628, 1.8133, 2.46198, 2.99801]]]

ypoints = [4, 25, 500, 1000, 1500, 1750, 2000, 2250, 2500, 2750, 3000]
ypoints = list(map(lambda x: x**3, ypoints))

for i in range(len(labels)):
    plt.plot(ypoints, xpoints[i], label=labels[i])

plt.title("different methods")
plt.xlabel("$n^3$")
plt.ylabel("time, s")

plt.legend()
plt.gcf().canvas.mpl_connect('key_press_event', close_figure)
plt.show() 

for j in range(len(titles)):
    for i in range(len(labels_par)):
        plt.plot(ypoints, parallel_mul[j][i], label=labels_par[i])

    plt.title(titles[j])
    plt.xlabel("$n^3$")
    plt.ylabel("time, s")

    plt.legend()
    plt.gcf().canvas.mpl_connect('key_press_event', close_figure)
    plt.show() 
