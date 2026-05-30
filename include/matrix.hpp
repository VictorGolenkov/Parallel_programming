#pragma once

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <omp.h>

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
};

template <typename T>
Matrix<T> multiply_omp(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    if (lhs.cols() != rhs.rows())
        throw std::invalid_argument("Matrix dimensions do not match for multiplication");

    size_t n = lhs.rows();
    Matrix<T> result(n, rhs.cols());

#pragma omp parallel for collapse(2)
    for (int i = 0; i < (int)n; ++i)
    {
        for (int j = 0; j < (int)rhs.cols(); ++j)
        {
            T sum = T();
            for (size_t k = 0; k < lhs.cols(); ++k)
                sum += lhs(i, k) * rhs(k, j);
            result(i, j) = sum;
        }
    }
    return result;
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