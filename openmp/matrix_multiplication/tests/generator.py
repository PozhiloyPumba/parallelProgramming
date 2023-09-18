import numpy as np
import sys

if (len(sys.argv) != 3):
    print("give me two filenames")
    exit()

n = int(input())
m1 = np.random.rand(n, n)
m2 = np.random.rand(n, n)

with open(sys.argv[1], 'w') as f:
    print(n, n, file=f)
    for i in m1:
        print(*i, file=f)

    print(n, n, file=f)
    for i in m2:
        print(*i, file=f)

with open(sys.argv[2], 'w') as f:
    for i in m1.dot(m2):
        print(*i, file=f)
