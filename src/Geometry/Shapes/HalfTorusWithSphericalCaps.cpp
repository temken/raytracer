#include "Geometry/Shapes/HalfTorusWithSphericalCaps.hpp"

#include "Geometry/Shapes/HalfSphere.hpp"
#include "Geometry/Shapes/HalfTorus.hpp"

#include <iostream>
#include <memory>

namespace Raytracer::Geometry {

HalfTorusWithSphericalCaps::HalfTorusWithSphericalCaps(const Vector3D& position, const Vector3D& orientation, const Vector3D& referenceDirection, double majorRadius, double minorRadius) :
    CompositeShape(Shape::Type::CYLINDER, position, orientation),
    mMajorRadius(majorRadius),
    mMinorRadius(minorRadius) {
    mOrthonormalBasis = OrthonormalBasis(orientation, referenceDirection);
    ComposeShape();
}

void HalfTorusWithSphericalCaps::PrintInfo() const {
    PrintInfoCompositeBase();
    std::cout << "\tReference Direction: " << mOrthonormalBasis.GetBasisVector(OrthonormalBasis::BasisVector::eX) << std::endl
              << "\tMajor Radius:\t" << mMajorRadius << std::endl
              << "\tMinor Radius:\t" << mMinorRadius << std::endl
              << std::endl;
}

void HalfTorusWithSphericalCaps::ComposeShape() {
    auto eZ = GetOrientation();
    auto eX = mOrthonormalBasis.GetBasisVector(OrthonormalBasis::BasisVector::eX);
    auto eY = mOrthonormalBasis.GetBasisVector(OrthonormalBasis::BasisVector::eY);

    // Create and add half-torus
    auto halfTorus = std::make_shared<HalfTorus>(mPosition, eZ, eX, mMajorRadius, mMinorRadius);
    AddComponent(halfTorus);

    // Create and add right spherical cap
    Vector3D rigthCenter = mPosition + eX * mMajorRadius;
    auto topSphere = std::make_shared<HalfSphere>(rigthCenter, mMinorRadius, -1.0 * eY);
    AddComponent(topSphere);

    // Create and add left spherical cap
    Vector3D leftCenter = mPosition - eX * mMajorRadius;
    auto bottomSphere = std::make_shared<HalfSphere>(leftCenter, mMinorRadius, -1.0 * eY);
    AddComponent(bottomSphere);
}

}  // namespace Raytracer::Geometry
