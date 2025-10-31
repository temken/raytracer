#include "Geometry/OrthonormalBasis.hpp"

namespace Raytracer {

OrthonormalBasis::OrthonormalBasis(Vector3D eZ, Vector3D eX) {
    eZ.Normalize();
    if (eX.NormSquared() < 1e-8) {
        Vector3D a = (std::fabs(eZ[0]) > 0.707) ? Vector3D{0.0, 1.0, 0.0} : Vector3D{1.0, 0.0, 0.0};
        eX = eZ.Cross(a);
    }
    eX.Normalize();
    Vector3D eY = eZ.Cross(eX).Normalized();

    mBasisVectors = {eX, eY, eZ};
}

const Vector3D& OrthonormalBasis::GetBasisVector(BasisVector axis) const {
    return mBasisVectors[static_cast<size_t>(axis)];
}

Vector3D OrthonormalBasis::ToLocal(const Vector3D& vectorInGlobal) const {
    return Vector3D({vectorInGlobal.Dot(GetBasisVector(BasisVector::eX)),
                     vectorInGlobal.Dot(GetBasisVector(BasisVector::eY)),
                     vectorInGlobal.Dot(GetBasisVector(BasisVector::eZ))});
}

Vector3D OrthonormalBasis::ToGlobal(const Vector3D& vectorInLocal) const {
    return GetBasisVector(BasisVector::eX) * vectorInLocal[0] +
           GetBasisVector(BasisVector::eY) * vectorInLocal[1] +
           GetBasisVector(BasisVector::eZ) * vectorInLocal[2];
}

void OrthonormalBasis::Rotate(double angle, const Vector3D& axis) {
    double cosAngle = std::cos(angle);
    double sinAngle = std::sin(angle);
    Vector3D normAxis = axis.Normalized();

    for (auto& basisVector : mBasisVectors) {
        basisVector = basisVector * cosAngle +
                      normAxis * (normAxis.Dot(basisVector)) * (1.0 - cosAngle) +
                      normAxis.Cross(basisVector) * sinAngle;
    }
}

void OrthonormalBasis::Rotate(double angle, BasisVector axis) {
    Rotate(angle, GetBasisVector(axis));
}

void OrthonormalBasis::PrintInfo() const {
    std::cout << "Orthonormal Basis Vectors:" << std::endl;
    std::cout << "\teX: " << GetBasisVector(BasisVector::eX) << std::endl;
    std::cout << "\teY: " << GetBasisVector(BasisVector::eY) << std::endl;
    std::cout << "\teZ: " << GetBasisVector(BasisVector::eZ) << std::endl;
}

}  // namespace Raytracer
