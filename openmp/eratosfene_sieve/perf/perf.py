import subprocess, os
import matplotlib.pyplot as plt

def close_figure(event):
    if event.key == 'escape':
        plt.close(event.canvas.figure)

executables_omp = ['par_for', 'par_task']
executables_seq = ['seq']

times = {k: [] for k in executables_omp + executables_seq}

for exec in executables_omp:
    num_threads = [1, 2, 3, 4, 5, 6, 7, 8]
    for num in num_threads:
        os.environ['OMP_NUM_THREADS'] = str(num)
        p1 = subprocess.Popen(["make", f"run_perf_{exec}"], stdout=subprocess.PIPE)
        output = p1.communicate()[0].decode("utf-8").split()
        times[exec].append(int(output[output.index("time:") + 1]))

for exec in executables_seq:
    p1 = subprocess.Popen(["make", f"run_perf_{exec}"], stdout=subprocess.PIPE)
    output = p1.communicate()[0].decode("utf-8").split() 
    t = int(output[output.index("time:") + 1])
    times[exec].append(t)

print(times)

for result in times.items():
    if (result[0] == 'seq'):
        continue

    plt.plot(range(1, len(result[1]) + 1), result[1])
    plt.plot(range(1, len(result[1]) + 1), [times['seq'][0] for _ in range(len(result[1]))])

    plt.title(f"Time {result[0]}")
    plt.xlabel("num threads")
    plt.ylabel("time, s")

    plt.gcf().canvas.mpl_connect('key_press_event', close_figure)
    plt.show()
# -----------------------------------
    speedup = list(map(lambda x: times['seq'][0] / x, result[1]))
    plt.plot(range(1, len(result[1]) + 1), speedup)

    plt.title(f"Speedup {result[0]}")
    plt.xlabel("num threads")
    plt.ylabel("Speedup")

    plt.gcf().canvas.mpl_connect('key_press_event', close_figure)
    plt.show()
# -----------------------------------
    eff = list(map(lambda x: x[1] / (x[0] + 1), enumerate(speedup)))
    plt.plot(range(1, len(result[1]) + 1), eff)

    plt.title(f"Efficiency {result[0]}")
    plt.xlabel("num threads")
    plt.ylabel("Efficiency")

    plt.gcf().canvas.mpl_connect('key_press_event', close_figure)
    plt.show()