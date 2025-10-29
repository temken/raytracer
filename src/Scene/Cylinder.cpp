#include "Scene/Cylinder.hpp"

#include "Scene/Tube.hpp"
#include "Scene/Disk.hpp"

#include <iostream>
#include <memory>

namespace Raytracer {

Cylinder::Cylinder(const std::string& name, const Material& material, const Vector3D& center, const Vector3D& normal, double radius, double height) :
    CompositeObject(name, material, center, normal),
    mRadius(radius),
    mHeight(height) {
    // mTopDisk("top_cap", material, center + (height / 2.0) * mNormal, mNormal, radius),
    // mBottomDisk("bottom_cap", material, center - (height / 2.0) * mNormal, -1.0 * mNormal, radius)
    // Create and add top disk
    Vector3D topCenter = mPosition + (mHeight / 2.0) * mNormal;
    auto topDisk = std::make_shared<Disk>("top_cap", material, topCenter, mNormal, mRadius);
    AddComponent(topDisk);
    // Create and add bottom disk
    Vector3D bottomCenter = mPosition - (mHeight / 2.0) * mNormal;
    auto bottomDisk = std::make_shared<Disk>("bottom_cap", material, bottomCenter, -1.0 * mNormal, mRadius);
    AddComponent(bottomDisk);
    // Create and add mantle (open cylinder)
    auto mantle = std::make_shared<Tube>("mantle", material, center, normal, radius, height);
    AddComponent(mantle);
}

void Cylinder::PrintInfo() const {
    PrintCompositeInfo();
    std::cout << "Shape:\tCylinder" << std::endl
              << "\tRadius: " << mRadius << std::endl
              << "\tHeight: " << mHeight << std::endl;
    mMaterial.PrintInfo();
}

}  // namespace Raytracer
