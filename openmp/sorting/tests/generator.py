import numpy as np
import sys

n = int(input())
arr = np.random.randint(-1000000, 1000000, size=(n))

print(n)
print(*arr)