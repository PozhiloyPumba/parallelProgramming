import subprocess, os
import matplotlib.pyplot as plt

def close_figure(event):
    if event.key == 'escape':
        plt.close(event.canvas.figure)

os.environ['OMP_NUM_THREADS'] = '8'

executables_omp = ['seq_base_mul', 'parallel_block_mul']
executables_opencl = ['opencl_base', 'opencl_cache']

n = [128, 256, 512, 768, 1024, 1536, 2048, 3072, 4096, 8192]
n_for_opencl = n
n_for_omp = n[:6]

processes = []

for i in n:
    processes.append(subprocess.Popen(["python", "../samples/generator.py", f"src{i}.txt", str(i)]))

exit_codes = [p.wait() for p in processes]

times = {k: [] for k in executables_omp + executables_opencl}

for exec in executables_omp:
    for i in n_for_omp:
        with open(f"src{i}.txt", "r") as input_file:
            p1 = subprocess.Popen(["make", f"run_{exec}"], stdin=input_file, stdout=subprocess.PIPE)
            output = p1.communicate()[0].decode("utf-8").split()
            times[exec].append(int(output[output.index("time:") + 1]))

for exec in executables_opencl:
    for i in n_for_opencl:
        with open(f"src{i}.txt", "r") as input_file:
            p1 = subprocess.Popen(["make", f"run_{exec}"], stdin=input_file, stdout=subprocess.PIPE)
            output = p1.communicate()[0].decode("utf-8").split()
            times[exec].append(int(output[output.index("time:") + 1]))

print(times)

for result in times.items():
    plt.plot(n_for_omp, result[1][:len(n_for_omp)], label=result[0])

plt.title(f"Time comparison between CPU and GPU")
plt.xlabel("matrix size")
plt.ylabel("time, s")
plt.legend()

plt.gcf().canvas.mpl_connect('key_press_event', close_figure)
plt.show()

for result in times.items():
    if result[0] in executables_opencl:
        plt.plot(n_for_opencl, result[1], label=result[0])

plt.title(f"Time comparison GPU simple and cache realization")
plt.xlabel("matrix size")
plt.ylabel("time, s")
plt.legend()

plt.gcf().canvas.mpl_connect('key_press_event', close_figure)
plt.show()