import subprocess, os
import matplotlib.pyplot as plt

def close_figure(event):
    if event.key == 'escape':
        plt.close(event.canvas.figure)

executables_omp = ['etalon', '2b', '3c']
executables_mpi = ['1a']

times = {}

for exec in executables_omp:
    times[exec] = [0]
    num_threads = [2, 3, 4, 5, 6, 7, 8]
    for num in num_threads:
        os.environ['OMP_NUM_THREADS'] = str(num)
        p1 = subprocess.Popen(["./samples/" + exec, ""], stdout=subprocess.PIPE)
        output = p1.communicate()[0].decode("utf-8").split() 
        if(output[-1] != "OK!"):
            print(f'error in {exec}')
            exit()
        times[exec][0] += float(output[3]) / len(num_threads)
        times[exec].append(float(output[1]))

for exec in executables_mpi:
    times[exec] = [0]
    num_threads = [2, 3, 4]
    for num in num_threads:
        p1 = subprocess.Popen(["mpiexec", "-np", str(num), "./samples/" + exec], stdout=subprocess.PIPE)
        output = p1.communicate()[0].decode("utf-8").split() 
        if(output[-1] != "OK!"):
            print(f'error in {exec}')
            exit()
        times[exec][0] += float(output[3]) / len(num_threads)
        times[exec].append(float(output[1]))

for result in times.items():
    plt.plot(range(1, len(result[1]) + 1), result[1])

    plt.title(f"Time {result[0]}")
    plt.xlabel("num threads")
    plt.ylabel("time, s")

    plt.gcf().canvas.mpl_connect('key_press_event', close_figure)
    plt.show()
# -----------------------------------
    speedup = list(map(lambda x: result[1][0] / x, result[1]))
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