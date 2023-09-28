import sys, os, getopt, subprocess
import matplotlib.pyplot as plt
import numpy as np

f = open("result.txt", "r")
out = f.read().split()
print(out)
times = [float(out[i + 1].split('m')[1].split('s')[0]) for i in range(len(out)) if out[i] == 'real']

print(times)
seq = times[::5]
par_1 = times[1::5]
par_2 = times[2::5]
par_3 = times[3::5]
par_4 = times[4::5]

fig, ax = plt.subplots()
num = [2**i for i in range(7, 17)]

line, = ax.plot(num, seq, linewidth=2.0, color='r')
line.set_label('seq')
line, = ax.plot(num, par_1, linewidth=2.0, color='b')
line.set_label('par_1')
line, = ax.plot(num, par_2, linewidth=2.0, color='g')
line.set_label('par_2')
line, = ax.plot(num, par_3, linewidth=2.0, color='y')
line.set_label('par_3')
line, = ax.plot(num, par_4, linewidth=2.0)
line.set_label('par_4')

ax.set_xlabel('num')
ax.set_ylabel('time')

#ax.plot(num, speedup, linewidth=2.0, color='r')
#ax.set_xscale('linear')

plt.show()
