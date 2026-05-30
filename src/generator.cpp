#include <fstream>
#include <random>
#include <string>
#include <iostream>
#include <cstdlib>

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
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <size1> [size2] ..." << std::endl;
        return 1;
    }

    for (int i = 1; i < argc; ++i)
    {
        size_t size = std::stoull(argv[i]);
        std::string a_file = std::string(PROJECT_ROOT) + "/input/A.txt";
        std::string b_file = std::string(PROJECT_ROOT) + "/input/B.txt";

        generate_matrix_file(a_file, size);
        generate_matrix_file(b_file, size);

    }

    return 0;
}