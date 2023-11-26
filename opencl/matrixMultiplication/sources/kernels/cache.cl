#ifndef TILE_SIZE_K
#define TILE_SIZE_K 1
#endif

#ifndef TILE_SIZE_ROW_C
#define TILE_SIZE_ROW_C 1
#endif

#ifndef TILE_SIZE_COL_C
#define TILE_SIZE_COL_C 1
#endif

#ifndef KERNEL_TYPE
#define KERNEL_TYPE int
#endif

__kernel void matmul(__global KERNEL_TYPE *A, __global KERNEL_TYPE *B, __global KERNEL_TYPE *C, int A_nCols, int B_nCols)
{
    int row = get_local_id(0);
    int col = get_local_id(1);

    int global_row = get_global_id(0);
    int global_col = get_global_id(1);

    KERNEL_TYPE acc = 0;
    __local KERNEL_TYPE A_Tile[TILE_SIZE_COL_C][TILE_SIZE_K];
    __local KERNEL_TYPE B_Tile[TILE_SIZE_K][TILE_SIZE_ROW_C];

    int numTiles = A_nCols / TILE_SIZE_K;

    for (int t = 0; t < numTiles; ++t) {
        const int tiled_row = TILE_SIZE_K * t + row % TILE_SIZE_K;
        const int tiled_col = TILE_SIZE_K * t + col % TILE_SIZE_K;
        A_Tile[row][col % TILE_SIZE_K] = A[global_row * A_nCols + tiled_col];
        B_Tile[row % TILE_SIZE_K][col] = B[tiled_row * B_nCols + global_col];

        barrier(CLK_LOCAL_MEM_FENCE);

        for (int k = 0; k < TILE_SIZE_K; ++k)
            acc += A_Tile[row][k] * B_Tile[k][col];

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    C[global_row * B_nCols + global_col] = acc;
}