#include <chrono>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cuda_runtime.h>

#include "matrix.hpp"

extern "C" void launchMatrixMulNaive(const double* d_A, const double* d_B, double* d_C, int N, int blockSize);
extern "C" void launchMatrixMulTiled(const double* d_A, const double* d_B, double* d_C, int N, int tileSize);

void warmupGPU()
{
	double* d_A, * d_B, * d_C;
	cudaMalloc(&d_A, 128 * 128 * sizeof(double));
	cudaMalloc(&d_B, 128 * 128 * sizeof(double));
	cudaMalloc(&d_C, 128 * 128 * sizeof(double));

	launchMatrixMulNaive(d_A, d_B, d_C, 128, 16);
	cudaDeviceSynchronize();

	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);
}

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			std::cerr << "Usage: " << argv[0] << " <size1> [size2] ..." << std::endl;
			return 1;
		}

		int deviceCount;
		cudaGetDeviceCount(&deviceCount);
		if (deviceCount == 0)
		{
			std::cerr << "No CUDA-capable GPU found!" << std::endl;
			return 1;
		}

		warmupGPU();


		std::vector<int> blockSizes = { 8, 16, 32 };

		std::cout << "Size;BlockSize;Algorithm;Time" << std::endl;

		for (int i = 1; i < argc; ++i)
		{
			size_t N = std::stoull(argv[i]);

			Matrix<double> A(std::string(PROJECT_ROOT) + "/input/A.txt");
			Matrix<double> B(std::string(PROJECT_ROOT) + "/input/B.txt");

			double* d_A, * d_B, * d_C;
			cudaMalloc(&d_A, N * N * sizeof(double));
			cudaMalloc(&d_B, N * N * sizeof(double));
			cudaMalloc(&d_C, N * N * sizeof(double));

			cudaMemcpy(d_A, A.data(), N * N * sizeof(double), cudaMemcpyHostToDevice);
			cudaMemcpy(d_B, B.data(), N * N * sizeof(double), cudaMemcpyHostToDevice);

			for (int blockSize : blockSizes)
			{
				cudaDeviceSynchronize();
				auto start = std::chrono::high_resolution_clock::now();

				launchMatrixMulNaive(d_A, d_B, d_C, (int)N, blockSize);
				cudaDeviceSynchronize();

				auto end = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration<double>(end - start);

				std::cout << N << ";" << blockSize << "x" << blockSize << ";Naive;"
					<< duration.count() << std::endl;
			}

			for (int tileSize : blockSizes)
			{
				cudaDeviceSynchronize();
				auto start = std::chrono::high_resolution_clock::now();

				launchMatrixMulTiled(d_A, d_B, d_C, (int)N, tileSize);
				cudaDeviceSynchronize();

				auto end = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration<double>(end - start);

				std::cout << N << ";" << tileSize << "x" << tileSize << ";Tiled;"
					<< duration.count() << std::endl;
			}

			Matrix<double> C(N, N);
			cudaMemcpy(C.data(), d_C, N * N * sizeof(double), cudaMemcpyDeviceToHost);

			std::string outpath = std::string(PROJECT_ROOT) + "/output/C.txt";
			std::ofstream file(outpath.c_str());
			if (!file.is_open())
				throw std::runtime_error("Could not open output file!");
			file << C.rows() << " " << C.cols() << std::endl;
			file << C;


			cudaFree(d_A);
			cudaFree(d_B);
			cudaFree(d_C);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}