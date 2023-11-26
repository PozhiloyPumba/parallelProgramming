#ifndef KERNEL_TYPE
#define KERNEL_TYPE int
#endif

__kernel void matmul(__global KERNEL_TYPE *A, __global KERNEL_TYPE *B, __global KERNEL_TYPE *C, int A_nCols, int B_nCols)
{
    int row = get_global_id(0);
    int col = get_global_id(1);

    KERNEL_TYPE acc = 0;

    for (int k = 0; k < A_nCols; ++k) {
        acc += A[row * A_nCols + k] * B[k * B_nCols + col];
    }

    C[row * B_nCols + col] = acc;
}