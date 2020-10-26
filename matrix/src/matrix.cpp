#include "matrix.h"
#include <algorithm>
#include <utility>
#include <cmath>

using namespace task;

Matrix::Row::Row(Iterator begin, Iterator end): _begin(begin), _end(end) {

}

double& Matrix::Row::operator[](size_t col) {
    return *(_begin + col);
}

const double& Matrix::Row::operator[](size_t col) const{
    return *(_begin + col);
}

Matrix::Row::Iterator Matrix::Row::begin() {
    return _begin;
}

Matrix::Row::Iterator Matrix::Row::end() {
    return _end;
}

const Matrix::Row::Iterator Matrix::Row::begin() const {
    return _begin;
}

const Matrix::Row::Iterator Matrix::Row::end() const {
    return _end;
}

auto Matrix::getIdx(size_t row, size_t col) const {
    return row * _cols + col;
}

void Matrix::checkSizes(const Matrix& other) const {
    if (_cols != other._cols || _rows != other._rows) throw SizeMismatchException();
}

Matrix::Matrix(): _rows(1), _cols(1), _data(std::make_unique<double []>(1)) {
    _data[0] = 1;
}

Matrix::Matrix(size_t rows, size_t cols): _rows(rows), _cols(cols),
            _data(std::make_unique<double []>(rows * cols)) {
    std::fill_n(_data.get(), rows * cols, 0.0);
    auto min_size = std::min(rows, cols);
    for (int i = 0; i < min_size; ++i) _data[getIdx(i, i)] = 1;
}

Matrix::Matrix(const Matrix& copy): _rows(copy._rows), _cols(copy._cols),
        _data(std::make_unique<double []>(copy._rows * copy._cols)) {
    std::copy(copy._data.get(), copy._data.get() + _rows * _cols, _data.get());
}
Matrix& Matrix::operator=(const Matrix& a) {
    if (this != &a) {
        Matrix temp(a);
        _data = std::move(temp._data);
        _rows = a._rows;
        _cols = a._cols;
    }

    return *this;
}

void Matrix::checkBounds(size_t row, size_t col) const {
    if (row >= _rows || col >= _cols) throw OutOfBoundsException();
}

double& Matrix::get(size_t row, size_t col) {
    checkBounds(row, col);
    return _data[getIdx(row, col)];
}

const double& Matrix::get(size_t row, size_t col) const {
    checkBounds(row, col);
    return _data[getIdx(row, col)];
}

void Matrix::set(size_t row, size_t col, const double& value) {
    get(row, col) = value;
}

void Matrix::resize(size_t new_rows, size_t new_cols) {
    auto new_data = std::make_unique<double []>(new_rows * new_cols);
    std::fill_n(new_data.get(), new_rows * new_cols, 0.0);

    auto min_rows = std::min(_rows, new_rows), min_cols = std::min(_cols, new_cols);
    for (size_t i = 0; i < min_rows; ++i)
        for (size_t j = 0; j < min_cols; ++j)
            new_data[i * new_cols + j] = _data[getIdx(i, j)];

    _rows = new_rows, _cols = new_cols;
    _data = std::move(new_data);
}

Matrix::Row  Matrix::operator[](size_t row) {
    auto begin =_data.get() + row * _cols;
    return {begin, begin + _cols};
}

Matrix::Row  Matrix::operator[](size_t row) const {
    auto begin =_data.get() + row * _cols;
    return {begin, begin + _cols};
}

template <class Op> Matrix&  Matrix::applyOp(const Matrix& other, Op op) {
    checkSizes(other);
    std::transform(_data.get(), _data.get() + _rows * _cols, other._data.get(), _data.get(), op);
    return *this;
}

Matrix& Matrix::operator+=(const Matrix& a) {
    return applyOp(a, std::plus<double>());
}

Matrix& Matrix::operator-=(const Matrix& a) {
    return applyOp(a, std::minus<double>());
}

Matrix& Matrix::operator*=(const Matrix& a) {
    return applyOp(a, std::multiplies<double>());
}

Matrix& Matrix::operator*=(const double& number) {
    std::transform(_data.get(), _data.get() + _rows * _cols, _data.get(), [number](auto item){
      return item * number;
    });

    return *this;
}

Matrix Matrix::operator+(const Matrix& a) const {
    return Matrix(*this) += a;
}

Matrix Matrix::operator-(const Matrix& a) const {
    return Matrix(*this) -= a;
}

Matrix Matrix::operator*(const Matrix& a) const {
    return Matrix(*this) *= a;
}

Matrix Matrix::operator*(const double& a) const {
    return Matrix(*this) *= a;
}

Matrix Matrix::operator-() const {
    return Matrix(*this) *= -1.0;
}

Matrix Matrix::operator+() const {
    return *this;
}

double  Matrix::det() const {
    if (_rows != _cols)
        throw SizeMismatchException();

    auto mat = *this;
    int det = 1, total = 1;
    auto n = _cols;

    for(size_t i = 0; i < n; ++i) {
        auto index = i;
        while(mat[index][i] == 0 && index < n) {
            ++index;
        }

        if(index == n)
            continue;

        else if(index != i)
        {
            for(int j = 0; j < n; ++j) {
                std::swap(mat[index][j], mat[i][j]);
            }

            det *= std::pow(-1, index - i);
        }

        auto row = mat.getRow(i);
        for(int j = i + 1; j < n; ++j)
        {
            auto num1 = row[i];
            auto num2 = mat[j][i];

            for(int k = 0; k < n; k++) {
                mat[j][k] = (num1 * mat[j][k]) - (num2 * row[k]);

            }
            total *= num1;
        }
    }

    for(int i = 0; i < n; i++) {
        det *= mat[i][i];
    }

    return det / total;
}

void  Matrix::transpose() {
    auto temp = this->transposed();

    _data = std::move(temp._data);
    std::swap(_rows, _cols);
}

Matrix  Matrix::transposed() const {
    Matrix res(_cols, _rows);

    for (size_t i = 0; i < _rows; ++i) {
        for (size_t j = 0; j < _cols; ++j)
            res._data[i + j * _rows] = _data[getIdx(i, j)];
    }

    return res;
}

double  Matrix::trace() const {
    if (_rows != _cols)
        throw SizeMismatchException();

    double res = 0;
    for (size_t i = 0; i < _cols; ++i) res += get(i, i);

    return res;
}

std::vector<double> Matrix::getRow(size_t row) {
    return {_data.get() + row * _cols, _data.get() + (row + 1) * _cols};
}

std::vector<double> Matrix::getColumn(size_t column) {
    std::vector<double> res(_rows);
    for (size_t i = 0; i < _rows; ++i) {
        res[i] = get(i, column);
    }

    return res;
}

bool Matrix::operator==(const Matrix& a) const {
    checkSizes(a);

    for (size_t i = 0; i < _cols * _rows; ++i) {
        if (std::abs(_data[i] - a._data[i]) >= EPS)
            return false;
    }

    return true;
}

bool Matrix::operator!=(const Matrix& a) const {
    return not (*this == a);
}


auto Matrix::getShape() const {
    return std::make_pair(_rows, _cols);
}

Matrix task::operator*(const double& a, const Matrix& b) {
    return Matrix(b) *= a;
}

std::ostream& task::operator<<(std::ostream& output, const Matrix& matrix) {
    auto [rows, cols] = matrix.getShape();
    for (size_t i = 0; i < rows; ++i) {
        for (auto item: matrix[i]) {
            output<<item<<' ';
        }
    }

    return output<<'\n';
}

std::istream& task::operator>>(std::istream& input, Matrix& matrix) {
    size_t rows, cols;
    input>>rows>>cols;
    matrix.resize(rows, cols);

    std::string temp;
    std::getline(input, temp);

    for (size_t i = 0; i < rows; ++i) {
        for (auto& item: matrix[i]) {
            input>>item;
        }
    }

    return input;
}
