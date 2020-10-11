#pragma once

#include <vector>
#include <iostream>
#include <memory>

namespace task {

const double EPS = 1e-6;


class OutOfBoundsException : public std::exception {};
class SizeMismatchException : public std::exception {};


class Matrix {
    class Row {
     public:
        using Iterator = double* const;
        Row(Iterator begin, Iterator end);
        double& operator[](size_t col);
        const double& operator[](size_t col) const;

        Iterator begin();
        Iterator end();

        const Iterator begin() const;
        const Iterator end() const;

     private:
        Iterator _begin, _end;
    };

 public:

    Matrix();
    Matrix(size_t rows, size_t cols);
    Matrix(const Matrix& copy);
    Matrix& operator=(const Matrix& a);

    double& get(size_t row, size_t col);
    const double& get(size_t row, size_t col) const;
    void set(size_t row, size_t col, const double& value);
    void resize(size_t new_rows, size_t new_cols);

    Row operator[](size_t row);
    Row operator[](size_t row) const;

    Matrix& operator+=(const Matrix& a);
    Matrix& operator-=(const Matrix& a);
    Matrix& operator*=(const Matrix& a);
    Matrix& operator*=(const double& number);

    Matrix operator+(const Matrix& a) const;
    Matrix operator-(const Matrix& a) const;
    Matrix operator*(const Matrix& a) const;
    Matrix operator*(const double& a) const;

    Matrix operator-() const;
    Matrix operator+() const;

    double det() const;
    void transpose();
    Matrix transposed() const;
    double trace() const;

    std::vector<double> getRow(size_t row);
    std::vector<double> getColumn(size_t column);

    bool operator==(const Matrix& a) const;
    bool operator!=(const Matrix& a) const;

    auto getShape() const;

 private:

    auto getIdx(size_t row, size_t col) const;
    void checkBounds(size_t row, size_t col) const;
    void checkSizes(const Matrix& other) const;

    template <class Op> Matrix& applyOp(const Matrix& other, Op op);

    size_t _rows, _cols;
    std::unique_ptr<double[]> _data;
};


Matrix operator*(const double& a, const Matrix& b);

std::ostream& operator<<(std::ostream& output, const Matrix& matrix);
std::istream& operator>>(std::istream& input, Matrix& matrix);



}  // namespace task
