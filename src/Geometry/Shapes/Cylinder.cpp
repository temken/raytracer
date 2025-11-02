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
    // Create and add top disk
    Vector3D topCenter = mPosition + (mHeight / 2.0) * orientation;
    auto topDisk = std::make_shared<Disk>(topCenter, orientation, mRadius);
    AddComponent(topDisk);
    // Create and add bottom disk
    Vector3D bottomCenter = mPosition - (mHeight / 2.0) * orientation;
    auto bottomDisk = std::make_shared<Disk>(bottomCenter, -1.0 * orientation, mRadius);
    AddComponent(bottomDisk);
    // Create and add mantle (open cylinder)
    auto mantle = std::make_shared<Tube>(position, orientation, radius, height);
    AddComponent(mantle);
}

void Cylinder::PrintInfo() const {
    PrintInfoCompositeBase();
    std::cout << "\tRadius: " << mRadius << std::endl
              << "\tHeight: " << mHeight << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
