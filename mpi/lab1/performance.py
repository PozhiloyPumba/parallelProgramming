import sys, os, getopt, subprocess
import matplotlib.pyplot as plt
import numpy as np

num = [32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144]
seq_time = [0.081, 0.062, 0.076, 0.067, 0.061, 0.111, 0.127, 0.207, 0.451, 0.935, 2.106, 4.147, 8.458, 16.989]
par_time = [0.067, 0.069, 0.074, 0.084, 0.087, 0.074, 0.100, 0.140, 0.258, 0.473, 1.150, 2.127, 4.417, 8.756]

speedup = list(np.array(seq_time)/np.array(par_time) / 4)

fig, ax = plt.subplots()

line, = ax.plot(num, speedup, linewidth=2.0, color='r')

#line, = ax.plot(num, par_time, linewidth=2.0, color='b')
#line.set_label('Parallel')
#ax.legend()

ax.set_xlabel('num')
ax.set_ylabel('efficiency')

#ax.plot(num, speedup, linewidth=2.0, color='r')
ax.set_xscale('linear')

plt.show()
