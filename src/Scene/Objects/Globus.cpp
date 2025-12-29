#include "Scene/Objects/Globus.hpp"

#include "Geometry/Shapes.hpp"
#include "Utilities/MaterialFactory.hpp"

namespace Raytracer::Items {

ObjectComposite CreateGlobus(double referenceLength, const Vector3D& position, const Vector3D& orientation, const Vector3D& referenceDirection) {
    ObjectComposite globus("Globus", referenceLength, position, orientation, referenceDirection);
    auto onb = Geometry::OrthonormalBasis(orientation, referenceDirection);

    Material brass = MaterialFactory::CreateBrass();
    Material copper = MaterialFactory::CreateCopper();
    Material silver = MaterialFactory::CreateSilver();

    // Stand: Bottom Disk
    double bottomDiskRadius = 0.5 * referenceLength;
    double bottomDiskHeight = 0.05 * referenceLength;
    Vector3D bottomDiskPosition = position + 0.5 * bottomDiskHeight * orientation.Normalized();
    auto bottomDisk = MakePrimitiveObject<Geometry::Cylinder>("bottomDisk", brass, bottomDiskPosition, orientation, bottomDiskRadius, bottomDiskHeight);
    globus.AddComponent(std::make_shared<ObjectPrimitive>(bottomDisk));

    // Stand: Small cylinder
    double standCylinderRadius = 0.05 * referenceLength;
    double standCylinderHeight = 0.2 * referenceLength;
    Vector3D standCylinderPosition = bottomDiskPosition + (bottomDiskHeight + 0.5 * standCylinderHeight) * orientation.Normalized();
    auto standCylinder = MakePrimitiveObject<Geometry::Cylinder>("standCylinder", brass, standCylinderPosition, orientation, standCylinderRadius, standCylinderHeight);
    globus.AddComponent(std::make_shared<ObjectPrimitive>(standCylinder));

    // Stand: Small spherical cap
    double capMaxAngle = M_PI / 6.0;
    double capSize = 0.9 * bottomDiskRadius;
    double capRadius = capSize / std::sin(capMaxAngle);
    Vector3D capPosition = position + (bottomDiskHeight - capRadius * std::cos(capMaxAngle)) * orientation.Normalized();
    auto standCap = MakePrimitiveObject<Geometry::SphericalCap>("standCap", brass, capPosition, capRadius, orientation, capMaxAngle);
    globus.AddComponent(std::make_shared<ObjectPrimitive>(standCap));

    // Half Torus with Spherical Caps
    double globeTilt = M_PI / 6.0;  // 30 degrees
    double torusMinorRadius = 0.02 * referenceLength;
    double distanceToGlobe = 0.1 * referenceLength;
    double torusMajorRadius = referenceLength + distanceToGlobe + torusMinorRadius;
    Vector3D center = position + onb.ToGlobal(Vector3D({0, 0, bottomDiskHeight + standCylinderHeight + 2.0 * torusMinorRadius + distanceToGlobe + referenceLength}));
    Vector3D halfTorusOrientation = onb.ToGlobal({1.0, 0.0, 0.0});
    Vector3D halfTorusReferenceDirection = onb.ToGlobal({0.0, -std::sin(globeTilt), std::cos(globeTilt)});
    auto halfTorusWithCaps = MakePrimitiveObject<Geometry::HalfTorusWithSphericalCaps>("globeHalfTorusWithCaps", copper, center, halfTorusOrientation, halfTorusReferenceDirection, torusMajorRadius, torusMinorRadius);
    globus.AddComponent(std::make_shared<ObjectPrimitive>(halfTorusWithCaps));

    // Tube
    double tubeRadius = 0.01 * referenceLength;
    double tubeLength = 2.05 * torusMajorRadius;
    Vector3D tubePosition = center;
    Vector3D tubeOrientation = onb.ToGlobal({0.0, -std::sin(globeTilt), std::cos(globeTilt)});
    auto tube = MakePrimitiveObject<Geometry::Tube>("globeTube", silver, tubePosition, tubeOrientation, tubeRadius, tubeLength);
    globus.AddComponent(std::make_shared<ObjectPrimitive>(tube));

    // Globe
    double globeRadius = referenceLength;
    Vector3D globePosition = center;
    Vector3D globeReferenceDirection = onb.ToGlobal({1.0, 0.0, 0.0});
    Material globeMaterial;
    globeMaterial.SetColorTexture("earth_vintage.png");
    auto globe = MakePrimitiveObject<Geometry::Sphere>("globe", globeMaterial, globePosition, globeRadius, tubeOrientation, globeReferenceDirection);
    globus.AddComponent(std::make_shared<ObjectPrimitive>(globe));

    // Equatorial ring
    double ringInnerRadius = globeRadius + 2.0 * torusMinorRadius;
    double ringOuterRadius = globeRadius + 8.0 * torusMinorRadius;
    double ringHeight = 0.4 * torusMinorRadius;
    auto equatorialRing = MakePrimitiveObject<Geometry::CylindricalShell>("equatorialRing", copper, center, tubeOrientation, ringInnerRadius, ringOuterRadius, ringHeight);
    globus.AddComponent(std::make_shared<ObjectPrimitive>(equatorialRing));

    // End spheres of the tube
    double sphereRadius = 2.0 * tubeRadius;
    Vector3D sphere1Position = tubePosition + 0.5 * tubeLength * tubeOrientation.Normalized();
    Vector3D sphere2Position = tubePosition - 0.5 * tubeLength * tubeOrientation.Normalized();
    auto endSphere1 = MakePrimitiveObject<Geometry::Sphere>("tubeEndSphere1", silver, sphere1Position, sphereRadius);
    auto endSphere2 = MakePrimitiveObject<Geometry::Sphere>("tubeEndSphere2", silver, sphere2Position, sphereRadius);
    globus.AddComponent(std::make_shared<ObjectPrimitive>(endSphere1));
    globus.AddComponent(std::make_shared<ObjectPrimitive>(endSphere2));

    return globus;
}

}  // namespace Raytracer::Items
