#pragma once

#include <array>
#include <cmath>
#include <format>
#include <initializer_list>
#include <iostream>
#include <stdexcept>

namespace Raytracer {

template <std::size_t N, typename T = double>
class Vector {
public:
    Vector() = default;

    explicit Vector(const std::array<T, N>& values) :
        data(values) {}

    Vector(std::initializer_list<T> values) {
        if (values.size() != N) {
            throw std::invalid_argument("Initializer list size must match dimension");
        }
        std::copy(values.begin(), values.end(), data.begin());
    }

    // Element access
    T& operator[](std::size_t i) {
        return data[i];
    }
    const T& operator[](std::size_t i) const {
        return data[i];
    }

    // Basic arithmetic
    Vector& operator+=(const Vector& other) {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] += other[i];
        }
        return *this;
    }

    Vector& operator-=(const Vector& other) {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] -= other[i];
        }
        return *this;
    }

    Vector& operator*=(const T& scalar) {
        for (auto& x : data) {
            x *= scalar;
        }
        return *this;
    }

    Vector& operator/=(const T& scalar) {
        for (auto& x : data) {
            x /= scalar;
        }
        return *this;
    }

    bool operator==(const Vector& other) const {
        return data == other.data;
    }

    // Norms
    T Dot(const Vector& other) const {
        T result{};
        for (std::size_t i = 0; i < N; ++i) {
            result += data[i] * other[i];
        }
        return result;
    }

    T NormSquared() const {
        return Dot(*this);
    }

    T Norm() const {
        return std::sqrt(NormSquared());
    }

    void Normalize() {
        T n = Norm();
        if (n == T{}) {
            throw std::runtime_error("Cannot normalize zero vector");
        }
        *this /= n;
    }

    [[nodiscard]] Vector Normalized() const {
        T n = Norm();
        if (n == T{}) {
            throw std::runtime_error("Cannot normalize zero vector");
        }
        Vector v = *this;
        v /= n;
        return v;
    }

    // Cross product
    Vector Cross(const Vector& other) const {
        static_assert(N == 3, "Cross product is only defined in 3D");
        return Vector{data[1] * other[2] - data[2] * other[1],
                      data[2] * other[0] - data[0] * other[2],
                      data[0] * other[1] - data[1] * other[0]};
    }

    // Iterators
    auto begin() {
        return data.begin();
    }
    auto end() {
        return data.end();
    }
    auto begin() const {
        return data.begin();
    }
    auto end() const {
        return data.end();
    }

private:
    std::array<T, N> data{};
};

using Vector2D = Vector<2, double>;
using Vector3D = Vector<3, double>;

// Free operators
template <std::size_t N, typename T>
Vector<N, T> operator+(Vector<N, T> lhs, const Vector<N, T>& rhs) {
    lhs += rhs;
    return lhs;
}

template <std::size_t N, typename T>
Vector<N, T> operator-(Vector<N, T> lhs, const Vector<N, T>& rhs) {
    lhs -= rhs;
    return lhs;
}

template <std::size_t N, typename T>
Vector<N, T> operator*(Vector<N, T> v, const T& scalar) {
    v *= scalar;
    return v;
}

template <std::size_t N, typename T>
Vector<N, T> operator*(const T& scalar, Vector<N, T> v) {
    v *= scalar;
    return v;
}

template <std::size_t N, typename T>
Vector<N, T> operator/(Vector<N, T> v, const T& scalar) {
    v /= scalar;
    return v;
}

// Stream output
template <std::size_t N, typename T>
std::ostream& operator<<(std::ostream& os, const Vector<N, T>& v) {
    os << "(";
    for (std::size_t i = 0; i < N; ++i) {
        os << v[i];
        if (i + 1 < N) {
            os << ", ";
        }
    }
    os << ")";
    return os;
}

}  // namespace Raytracer
