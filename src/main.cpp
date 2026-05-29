#include <mpi.h>
#include <chrono>
#include <cstdlib>
#include <random>
#include <stdexcept>

#include "matrix.hpp"

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int rank, world_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	try
	{

		//for (int i = 1; i < argc; ++i)
		//{
		size_t size = std::stoull(argv[1]);

		Matrix<double> A(PROJECT_ROOT "/input/A.txt");
		Matrix<double> B(PROJECT_ROOT "/input/B.txt");

		MPI_Barrier(MPI_COMM_WORLD);
		auto start = MPI_Wtime();

		Matrix<double> C = multiply_mpi(A, B);

		auto end = MPI_Wtime();
		auto duration = end - start;

		if (rank == 0)
		{

			std::cout << size << ";" << world_size << ";" << duration << std::endl;

			std::string outpath = PROJECT_ROOT "/output/C.txt";
			std::ofstream file(outpath);
			if (!file.is_open())
				throw std::runtime_error("Could not open output file!");
			file << C.rows() << " " << C.cols() << std::endl;
			file << C;
		}
	}
	/* }*/
	catch (const std::exception& e)
	{
		std::cerr << "Error on rank " << rank << ": " << e.what() << std::endl;
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	MPI_Finalize();
	return 0;
}