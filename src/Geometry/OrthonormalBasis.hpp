#pragma once

#include "Geometry/Vector.hpp"

#include <array>

namespace Raytracer::Geometry {

class OrthonormalBasis {
public:
    enum class BasisVector {
        eX = 0,
        eY = 1,
        eZ = 2
    };

    OrthonormalBasis(Vector3D eZ, Vector3D eX = Vector3D({0.0, 0.0, 0.0}));

    const Vector3D& GetBasisVector(BasisVector axis) const;

    Vector3D ToLocal(const Vector3D& vectorInGlobal) const;
    Vector3D ToGlobal(const Vector3D& vectorInLocal) const;

    void Rotate(double angle, const Vector3D& axis);
    void Rotate(double angle, BasisVector axis);

    // Iterator support for range-based loops
    auto begin() const {
        return mBasisVectors.begin();
    }
    auto end() const {
        return mBasisVectors.end();
    }
    auto begin() {
        return mBasisVectors.begin();
    }
    auto end() {
        return mBasisVectors.end();
    }

    // Array-style access
    const Vector3D& operator[](size_t index) const {
        return mBasisVectors[index];
    }
    Vector3D& operator[](size_t index) {
        return mBasisVectors[index];
    }

    void PrintInfo() const;

private:
    std::array<Vector3D, 3> mBasisVectors;  // eX, eY, eZ
};

}  // namespace Raytracer::Geometry
