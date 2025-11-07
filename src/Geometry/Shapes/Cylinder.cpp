#include "Geometry/Shapes/Cylinder.hpp"

#include "Geometry/Shapes/Disk.hpp"
#include "Geometry/Shapes/Tube.hpp"

#include <iostream>
#include <memory>

namespace Raytracer::Geometry {

Cylinder::Cylinder(const Vector3D& position, const Vector3D& orientation, double radius, double height) :
    CompositeShape(Shape::Type::CYLINDER, position, orientation),
    mRadius(radius),
    mHeight(height) {
    ComposeShape();
}

void Cylinder::PrintInfo() const {
    PrintInfoCompositeBase();
    std::cout << "\tRadius: " << mRadius << std::endl
              << "\tHeight: " << mHeight << std::endl
              << std::endl;
}

void Cylinder::ComposeShape() {
    auto cylinderOrientation = GetOrientation();

    // Create and add top disk
    Vector3D topCenter = mPosition + (mHeight / 2.0) * cylinderOrientation;
    auto topDisk = std::make_shared<Disk>(topCenter, cylinderOrientation, mRadius);
    AddComponent(topDisk);
    // Create and add bottom disk
    Vector3D bottomCenter = mPosition - (mHeight / 2.0) * cylinderOrientation;
    auto bottomDisk = std::make_shared<Disk>(bottomCenter, -1.0 * cylinderOrientation, mRadius);
    AddComponent(bottomDisk);
    // Create and add mantle (open cylinder)
    auto mantle = std::make_shared<Tube>(mPosition, cylinderOrientation, mRadius, mHeight);
    AddComponent(mantle);
}

}  // namespace Raytracer::Geometry
