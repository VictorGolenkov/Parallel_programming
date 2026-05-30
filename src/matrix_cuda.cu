#include <cuda_runtime.h>

// Наивное умножение (только глобальная память)
__global__ void matrixMulNaive(const double* A, const double* B, double* C, int N)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < N && col < N)
    {
        double sum = 0.0;
        for (int k = 0; k < N; ++k)
        {
            sum += A[row * N + k] * B[k * N + col];
        }
        C[row * N + col] = sum;
    }
}

// Tiling-умножение с shared memory
#define TILE_SIZE 32

__global__ void matrixMulTiled(const double* A, const double* B, double* C, int N)
{
    __shared__ double tileA[TILE_SIZE][TILE_SIZE];
    __shared__ double tileB[TILE_SIZE][TILE_SIZE];

    int row = blockIdx.y * TILE_SIZE + threadIdx.y;
    int col = blockIdx.x * TILE_SIZE + threadIdx.x;

    double sum = 0.0;

    for (int t = 0; t < (N + TILE_SIZE - 1) / TILE_SIZE; ++t)
    {
        // Загрузка тайлов в shared memory
        int tiledRow = t * TILE_SIZE + threadIdx.y;
        int tiledCol = t * TILE_SIZE + threadIdx.x;

        if (row < N && tiledCol < N)
            tileA[threadIdx.y][threadIdx.x] = A[row * N + tiledCol];
        else
            tileA[threadIdx.y][threadIdx.x] = 0.0;

        if (tiledRow < N && col < N)
            tileB[threadIdx.y][threadIdx.x] = B[tiledRow * N + col];
        else
            tileB[threadIdx.y][threadIdx.x] = 0.0;

        __syncthreads();

        // Умножение тайлов
        for (int k = 0; k < TILE_SIZE; ++k)
        {
            sum += tileA[threadIdx.y][k] * tileB[k][threadIdx.x];
        }

        __syncthreads();
    }

    if (row < N && col < N)
    {
        C[row * N + col] = sum;
    }
}

// Интерфейсная функция для вызова из C++
extern "C" void launchMatrixMulNaive(const double* d_A, const double* d_B, double* d_C, int N, int blockSize)
{
    dim3 blockDim(blockSize, blockSize);
    dim3 gridDim((N + blockSize - 1) / blockSize, (N + blockSize - 1) / blockSize);
    
    matrixMulNaive<<<gridDim, blockDim>>>(d_A, d_B, d_C, N);
}

extern "C" void launchMatrixMulTiled(const double* d_A, const double* d_B, double* d_C, int N)
{
    dim3 blockDim(TILE_SIZE, TILE_SIZE);
    dim3 gridDim((N + TILE_SIZE - 1) / TILE_SIZE, (N + TILE_SIZE - 1) / TILE_SIZE);
    
    matrixMulTiled<<<gridDim, blockDim>>>(d_A, d_B, d_C, N);
}