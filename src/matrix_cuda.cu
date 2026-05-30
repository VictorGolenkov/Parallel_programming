#include <cuda_runtime.h>

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

__global__ void matrixMulTiled(const double* A, const double* B, double* C, int N, int tileSize)
{
    extern __shared__ double sharedMem[];
    double* tileA = sharedMem;
    double* tileB = &sharedMem[tileSize * tileSize];

    int row = blockIdx.y * tileSize + threadIdx.y;
    int col = blockIdx.x * tileSize + threadIdx.x;

    double sum = 0.0;

    for (int t = 0; t < (N + tileSize - 1) / tileSize; ++t)
    {
        int tiledRow = t * tileSize + threadIdx.y;
        int tiledCol = t * tileSize + threadIdx.x;

        if (row < N && tiledCol < N)
            tileA[threadIdx.y * tileSize + threadIdx.x] = A[row * N + tiledCol];
        else
            tileA[threadIdx.y * tileSize + threadIdx.x] = 0.0;

        if (tiledRow < N && col < N)
            tileB[threadIdx.y * tileSize + threadIdx.x] = B[tiledRow * N + col];
        else
            tileB[threadIdx.y * tileSize + threadIdx.x] = 0.0;

        __syncthreads();

        for (int k = 0; k < tileSize; ++k)
        {
            sum += tileA[threadIdx.y * tileSize + k] * tileB[k * tileSize + threadIdx.x];
        }

        __syncthreads();
    }

    if (row < N && col < N)
    {
        C[row * N + col] = sum;
    }
}

extern "C" void launchMatrixMulNaive(const double* d_A, const double* d_B, double* d_C, int N, int blockSize)
{
    dim3 blockDim(blockSize, blockSize);
    dim3 gridDim((N + blockSize - 1) / blockSize, (N + blockSize - 1) / blockSize);
    
    matrixMulNaive<<<gridDim, blockDim>>>(d_A, d_B, d_C, N);
}

extern "C" void launchMatrixMulTiled(const double* d_A, const double* d_B, double* d_C, int N, int tileSize)
{
    dim3 blockDim(tileSize, tileSize);
    dim3 gridDim((N + tileSize - 1) / tileSize, (N + tileSize - 1) / tileSize);

    size_t sharedMemSize = 2 * tileSize * tileSize * sizeof(double);

    matrixMulTiled<<<gridDim, blockDim, sharedMemSize>>>(d_A, d_B, d_C, N, tileSize);
}