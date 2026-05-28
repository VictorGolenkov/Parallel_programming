#include <chrono>

#include "matrix.hpp"

int main()
{
    try
    {
        Matrix<double> A(PROJECT_ROOT "/input/A.txt");
        Matrix<double> B(PROJECT_ROOT "/input/B.txt");

        auto start = std::chrono::high_resolution_clock::now();
        Matrix<double> C = A * B;
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - start);

        std::string outpath = PROJECT_ROOT "/output/C.txt";
        std::ofstream file(outpath);
        if (!file.is_open())
            throw std::runtime_error("Could not open output file!");
        file << "Matrix size:\t" << C.rows() << "x" << C.cols() << std::endl;
        file << "Computation time:\t" << duration.count() << " seconds" << std::endl;
        file << C << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
