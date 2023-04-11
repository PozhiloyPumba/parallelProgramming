import sys, os, getopt, subprocess
import matplotlib.pyplot as plt
import numpy as np

def run_sequential(c_file, t_size, x_size, exe_file = 'sequential.out'):
    command_gcc = 'gcc ' + c_file + f' -DQUIET -DK={t_size} ' + f' -DM={x_size} ' + '-O3 -lm -o ' + exe_file
    if os.system(command_gcc) == 0:
        os.system("echo Running " + exe_file)
        os.system("echo -------------------")
        return subprocess.getoutput(f"time ./{exe_file}")
    raise RuntimeError(f"don't compile {exe_file} fix it bro((")

def run_parallel(c_file, t_size, x_size, num_proc, exe_file = 'parallel.out'):
    command_mpicc = 'mpicc ' + c_file + f' -DQUIET -DK={t_size} ' + f' -DM={x_size} ' + '-O3 -lm -o ' + exe_file
    if os.system(command_mpicc) == 0:
        os.system("echo Running " + exe_file)
        os.system("echo -------------------")
        return subprocess.getoutput(f"time mpirun -n {num_proc} ./{exe_file}")
    raise RuntimeError(f"don't compile {exe_file} fix it bro((")

def gettime(x):
    m,s = map(float,x[:-1].split('m'))
    return 60 * m + s

try:
    num_proc = 4
    num = [2]
    seq_time = []
    par_time = []
    speedup = []
    for i in range(12):
        out = run_sequential('sequential.c', num[-1], 1001).split()
        #print(num[-1], gettime(out[out.index('real') + 1]))
        seq_time.append(gettime(out[out.index('real') + 1]))
        out = run_parallel('parallel.c', num[-1], 1001, num_proc).split()
        #print(num[-1], gettime(out[out.index('real') + 1]))
        par_time.append(gettime(out[out.index('real') + 1]))
        speedup.append(seq_time[-1] / par_time[-1])
        #print(num[-1])
        num.append(num[-1] * 2)
except Exception as se:
    print(str(se))

num.pop(-1)

fig, ax = plt.subplots()

ax.plot(num, seq_time, linewidth=2.0, color='r')
ax.plot(num, par_time, linewidth=2.0, color='b')
#ax.set_xscale('log')

#ax.plot(num, speedup, linewidth=2.0, color='r')
ax.set_xscale('log')

plt.show()
