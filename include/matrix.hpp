#pragma once

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <mpi.h>

template <typename T>
class Matrix
{
private:
    size_t _rows;
    size_t _cols;
    std::vector<T> _data;

public:
    explicit Matrix(size_t n) : Matrix(n, n) {}
    Matrix(size_t rows, size_t cols) : _rows(rows), _cols(cols), _data(rows* cols) {}
    explicit Matrix(const std::string& filename) : _rows(0), _cols(0)
    {
        std::ifstream file(filename);
        if (!file.is_open())
            throw std::runtime_error("Could not open file: " + filename);

        size_t rows = 0, cols = 0;
        if (!(file >> rows >> cols))
            throw std::runtime_error("Invalid matrix dimensions in file!");
        if (rows == 0 || cols == 0)
            throw std::runtime_error("Matrix dimensions must be positive!");

        _rows = rows;
        _cols = cols;
        _data.resize(rows * cols);

        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < cols; ++j)
            {
                if (!(file >> _data[i * cols + j]))
                    throw std::runtime_error("Invalid matrix data in file!");
            }
        }
    }

    T& operator()(size_t row, size_t col)
    {
        if (row >= _rows || col >= _cols)
            throw std::out_of_range("Matrix index out of range");
        return _data[row * _cols + col];
    }
    const T& operator()(size_t row, size_t col) const
    {
        if (row >= _rows || col >= _cols)
            throw std::out_of_range("Matrix index out of range");
        return _data[row * _cols + col];
    }

    size_t rows() const noexcept { return _rows; }
    size_t cols() const noexcept { return _cols; }
    const T* data() const noexcept { return _data.data(); }
    T* data() noexcept { return _data.data(); }
};

template <typename T>
Matrix<T> multiply_mpi(const Matrix<T>& A, const Matrix<T>& B)
{
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    size_t n = A.rows();
    if (A.cols() != n || B.rows() != n || B.cols() != n)
        throw std::invalid_argument("MPI multiplication requires square matrices");

    size_t rows_per_proc = n / size;
    size_t remainder = n % size;
    size_t local_rows = rows_per_proc + (rank < remainder ? 1 : 0);

    std::vector<int> sendcounts(size), displs(size);
    size_t offset = 0;
    for (int i = 0; i < size; ++i)
    {
        sendcounts[i] = (rows_per_proc + (i < remainder ? 1 : 0)) * n;
        displs[i] = offset;
        offset += sendcounts[i];
    }

    std::vector<T> local_A(local_rows * n);
    MPI_Scatterv(const_cast<T*>(A.data()), sendcounts.data(), displs.data(), MPI_DOUBLE,
             local_A.data(), (int)(local_rows * n), MPI_DOUBLE,
             0, MPI_COMM_WORLD);
    MPI_Bcast(const_cast<T*>(B.data()), n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    std::vector<T> local_C(local_rows * n, 0.0);
    for (size_t i = 0; i < local_rows; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            for (size_t k = 0; k < n; ++k)
            {
                local_C[i * n + j] += local_A[i * n + k] * B(k, j);
            }
        }
    }

    Matrix<T> C(n, n);
    MPI_Gatherv(local_C.data(), local_rows * n, MPI_DOUBLE,
        C.data(), sendcounts.data(), displs.data(), MPI_DOUBLE,
        0, MPI_COMM_WORLD);

    return C;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix)
{
    for (size_t i = 0; i < matrix.rows(); ++i)
    {
        for (size_t j = 0; j < matrix.cols(); ++j)
            os << matrix(i, j) << " ";
        os << "\n";
    }
    return os;
}