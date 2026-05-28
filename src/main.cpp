#include <chrono>
#include <cstdlib>
#include <random>

#include "matrix.hpp"

void generate_matrix_file(const std::string& filename, size_t size)
{
    std::ofstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Could not create file: " + filename);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-10.0, 10.0);

    file << size << " " << size << "\n";
    for (size_t i = 0; i < size; ++i)
    {
        for (size_t j = 0; j < size; ++j)
            file << dist(gen) << " ";
        file << "\n";
    }
}

int main(int argc, char* argv[])
{
    try
    {
        std::cout << "Square matrix size:" << ";" << "Computation time:" << std::endl;
        for (int i = 1; i < argc; ++i) {

            size_t size = std::stoull(argv[i]);

            generate_matrix_file(PROJECT_ROOT "/input/A.txt", size);
            generate_matrix_file(PROJECT_ROOT "/input/B.txt", size);

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

            std::cout << C.rows() << ";" << duration.count() << std::endl;

            file << C.rows() << " " << C.cols() << std::endl;
            file << C;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}