#pragma once
#include <vector>
#include <iostream>
#include <cassert>
#include <numeric>
#include <algorithm>

namespace task {

template<typename Tp> auto operator+(const std::vector<Tp>& vec) {
    return vec;
}

template<typename Tp> auto operator-(const std::vector<Tp>& vec) {
    auto res = decltype(vec){};
    res.reserve(vec.size());
    std::transform(std::begin(vec), std::end(vec), std::back_inserter(res), std::negate<Tp>());

    return res;
}

template<typename Tp, typename Op> auto
BinaryOp(const std::vector<Tp>& left, const std::vector<Tp>& right, Op op) {
    auto size = left.size();
    assert(size == right.size());

    auto res = decltype(left){};
    res.reserve(size);

    std::transform(std::begin(left), std::end(left), std::begin(right),
        std::back_inserter(res), op);

    return res;
}

template<typename Tp>
auto operator+(const std::vector<Tp>& left, const std::vector<Tp>& right) {
    return BinaryOp(left, right, std::plus<Tp>());
}

template<typename Tp>
auto operator-(const std::vector<Tp>& left, const std::vector<Tp>& right) {
    return BinaryOp(left, right, std::minus<Tp>());
}

template<typename Tp>
auto operator*(const std::vector<Tp>& left, const std::vector<Tp>& right) {
    auto res = BinaryOp(left, right, std::multiplies<Tp>());
    return std::accumulate(std::begin(res), std::end(res), Tp{});
}

template<typename Tp>
auto operator%(const std::vector<Tp>& left, const std::vector<Tp>& right) {
    assert(left.size() == 3 and right.size() == 3);
    return decltype(left) {left[1] * right[2] - left[2] * right[1], left[2] * right[0] - left[0] * right[2],
                           left[0] * right[1] - left[1] * right[0]};
}

template<typename Tp>
auto operator||(const std::vector<Tp>& left, const std::vector<Tp>& right) {
    constexpr Tp zero = {};
    constexpr Tp eps = Tp{0.0001f};
    static auto is_close = [eps](Tp x, Tp y){return abs(x - y) <= eps;};

    auto size = left.size();
    assert(size == right.size());

    Tp ratio = zero;
    auto found = false;
    for (size_t i = 0; i < size; ++i) {
        auto l = left[i], r = right[i];
        auto is_l_zero = is_close(l, zero), is_r_zero = is_close(r, zero);
        if (is_l_zero and is_r_zero) {
            continue;
        } else if (found) {
            if (not is_close(l, r * ratio))
                return 0;
        } else if (not is_l_zero and not is_r_zero) {
            found = true;
            ratio = l / r;
        }
        else {
            return 0;
        }
    }

    return ratio >= zero ? 1 : -1;
}

template<typename Tp>
auto operator&&(const std::vector<Tp>& left, const std::vector<Tp>& right) {
    return (left || right) == 1;
}

template<typename Tp>
std::istream& operator>>(std::istream& in, std::vector<Tp>& dest) {
    size_t n;
    in>>n;

    dest.resize(n);
    for (auto& item: dest)
        in >> item;

    return in;
}

template<typename Tp>
std::ostream& operator<<(std::ostream& out, const std::vector<Tp>& source) {
    auto size = source.size();
    for (size_t i = 0; i < size; ++i)
        out << source[i] << (i == size - 1 ? '\n' : ' ');

    return out;
}

template<typename Tp>
auto reverse(std::vector<Tp>& vec) {
    auto begin = std::begin(vec), end = std::end(vec);
    while (std::distance(begin, end) > 1) {
        std::iter_swap(begin++, --end);
    }
}

template<typename Tp>
auto operator|(const std::vector<Tp>& left, const std::vector<Tp>& right) {
    return BinaryOp(left, right, std::bit_or<Tp>());
}

template<typename Tp>
auto operator&(const std::vector<Tp>& left, const std::vector<Tp>& right) {
    return BinaryOp(left, right, std::bit_and<Tp>());
}

}  // namespace task
