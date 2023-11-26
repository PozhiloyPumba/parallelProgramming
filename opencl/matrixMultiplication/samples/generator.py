import numpy as np
import sys

if (len(sys.argv) != 3):
    print("give me two args")
    exit()

n = int(sys.argv[2])
m1 = np.random.rand(n, n)
m2 = np.random.rand(n, n)

with open(sys.argv[1], 'w') as f:
    print(n, n, file=f)
    for i in m1:
        print(*i, file=f)

    print(n, n, file=f)
    for i in m2:
        print(*i, file=f)
