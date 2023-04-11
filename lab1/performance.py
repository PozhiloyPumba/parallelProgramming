import sys, os, getopt, subprocess

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
    num = 1
    for i in range(18):
        num *= 2
        out = run_sequential('sequential.c', num, 1001).split()
        print(num, gettime(out[out.index('real') + 1]))
        out = run_parallel('parallel.c', num, 1001, 4).split()
        print(num, gettime(out[out.index('real') + 1]))   
except Exception as se:
    print(str(se))
