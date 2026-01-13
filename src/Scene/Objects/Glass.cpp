#include "Scene/Objects/Globus.hpp"

#include "Geometry/Shapes.hpp"
#include "Utilities/MaterialFactory.hpp"

namespace Raytracer::Items {

ObjectComposite CreateGlass(double referenceLength, const Vector3D& position, const Vector3D& orientation, const Vector3D& referenceDirection, Color glassColor, double fillingFraction, Color fillingColor) {
    ObjectComposite glass("Glass", referenceLength, position, orientation, referenceDirection);
    auto onb = Geometry::OrthonormalBasis(orientation, referenceDirection);

    Material glassMaterial = MaterialFactory::CreateGlass(0.0, glassColor);

    // Glass cylindric shell
    double shellRadius = 0.5 * referenceLength;
    double innerRadius = 0.9 * shellRadius;
    double shellHeight = referenceLength;
    Vector3D shellPosition = position + 0.5 * shellHeight * orientation.Normalized();
    auto glassShell = MakePrimitiveObject<Geometry::CylindricalShell>("glassShell", glassMaterial, shellPosition, orientation, innerRadius, shellRadius, shellHeight);
    glass.AddComponent(std::make_shared<ObjectPrimitive>(glassShell));

    // Bottom disk
    double bottomDiskRadius = innerRadius;
    double bottomDiskHeight = 0.05 * referenceLength;
    Vector3D bottomDiskPosition = position + 0.5 * bottomDiskHeight * orientation.Normalized();
    auto bottomDisk = MakePrimitiveObject<Geometry::Cylinder>("glassBottomDisk", glassMaterial, bottomDiskPosition, orientation, bottomDiskRadius, bottomDiskHeight);
    glass.AddComponent(std::make_shared<ObjectPrimitive>(bottomDisk));

    return glass;
}

}  // namespace Raytracer::Items
