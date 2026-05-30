#include <chrono>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <omp.h>

#include "matrix.hpp"

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 2)
        {
            std::cerr << "Usage: " << argv[0] << " <size1> [size2] ..." << std::endl;
            return 1;
        }

        std::cout << "Size;Threads;Time" << std::endl;

        int max_threads = omp_get_max_threads();
        std::vector<int> thread_counts;
        for (int t = 1; t <= max_threads; t *= 2)
            thread_counts.push_back(t);
        if (thread_counts.back() != max_threads)
            thread_counts.push_back(max_threads);

        for (int i = 1; i < argc; ++i)
        {
            size_t size = std::stoull(argv[i]);

            std::string a_file = std::string(PROJECT_ROOT) + "/input/A.txt";
            std::string b_file = std::string(PROJECT_ROOT) + "/input/B.txt";

            Matrix<double> A(a_file);
            Matrix<double> B(b_file);

            for (int threads : thread_counts)
            {
                omp_set_num_threads(threads);

                auto start = std::chrono::high_resolution_clock::now();
                Matrix<double> C = multiply_omp(A, B);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration<double>(end - start);

                std::cout << size << ";" << threads << ";" << duration.count() << std::endl;

                if (threads == max_threads)
                {
                    std::string outpath = std::string(PROJECT_ROOT) + "/output/C_omp.txt";
                    std::ofstream file(outpath.c_str());
                    if (!file.is_open())
                        throw std::runtime_error("Could not open output file!");
                    file << C.rows() << " " << C.cols() << std::endl;
                    file << C;
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}