#include "Geometry/Shapes/CylindricalShell.hpp"

#include "Geometry/Shapes/Ring.hpp"
#include "Geometry/Shapes/Tube.hpp"

#include <iostream>
#include <memory>

namespace Raytracer::Geometry {

CylindricalShell::CylindricalShell(const Vector3D& position, const Vector3D& orientation, double innerRadius, double outerRadius, double height) :
    CompositeShape(Shape::Type::CYLINDRICAL_SHELL, position, orientation),
    mInnerRadius(innerRadius),
    mOuterRadius(outerRadius),
    mHeight(height) {
    ComposeShape();
}

void CylindricalShell::PrintInfo() const {
    PrintInfoCompositeBase();
    std::cout << "\tInner Radius: " << mInnerRadius << std::endl
              << "\tOuter Radius: " << mOuterRadius << std::endl
              << "\tHeight: " << mHeight << std::endl
              << std::endl;
}

void CylindricalShell::ComposeShape() {
    auto orientation = GetOrientation();

    // Create and add top ring
    Vector3D topCenter = mPosition + (mHeight / 2.0) * orientation;
    auto topRing = std::make_shared<Ring>(topCenter, orientation, mInnerRadius, mOuterRadius);
    AddComponent(topRing);
    // Create and add bottom ring
    Vector3D bottomCenter = mPosition - (mHeight / 2.0) * orientation;
    auto bottomRing = std::make_shared<Ring>(bottomCenter, -1.0 * orientation, mInnerRadius, mOuterRadius);
    AddComponent(bottomRing);
    // Create and add inner tube
    auto innerTube = std::make_shared<Tube>(mPosition, orientation, mInnerRadius, mHeight);
    AddComponent(innerTube);
    // Create and add outer tube
    auto outerTube = std::make_shared<Tube>(mPosition, orientation, mOuterRadius, mHeight);
    AddComponent(outerTube);
}

}  // namespace Raytracer::Geometry
