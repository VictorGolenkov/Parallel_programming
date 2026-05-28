import numpy as np
import sys

def read_matrix(filename):
    with open(filename, 'r') as f:
        rows, cols = map(int, f.readline().split())
        data = [[float(num) for num in line.split()] for line in f]
    return np.array(data)

A = read_matrix("../input/A.txt")
B = read_matrix("../input/B.txt")
C = read_matrix("../output/C.txt")

C_expected = A @ B
# print(C)

if np.allclose(C, C_expected):
    print("Verification PASSED")
    print(f"  Max error: {np.max(np.abs(C - C_expected)):.2e}")
else:
    print("Verification FAILED")
    print(f"  Max error: {np.max(np.abs(C - C_expected)):.2e}")
    sys.exit(1)
