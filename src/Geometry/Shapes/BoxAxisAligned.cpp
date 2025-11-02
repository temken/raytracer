#include "Geometry/Shapes/BoxAxisAligned.hpp"

#include "Geometry/Shapes/Rectangle.hpp"

namespace Raytracer::Geometry {

BoxAxisAligned::BoxAxisAligned(const Vector3D& center, double length, double width, double height) :
    CompositeShape(Type::BOX_AXIS_ALIGNED, center, Vector3D({0.0, 0.0, 1.0}), Vector3D({1.0, 0.0, 0.0})),
    mLength(length),
    mWidth(width),
    mHeight(height) {
    // Create 6 rectangles for the box faces
    auto halfLength = length / 2.0;
    auto halfWidth = width / 2.0;
    auto halfHeight = height / 2.0;

    const Vector3D eX = GetBasisVector(OrthonormalBasis::BasisVector::eX);
    const Vector3D eY = GetBasisVector(OrthonormalBasis::BasisVector::eY);
    const Vector3D eZ = GetBasisVector(OrthonormalBasis::BasisVector::eZ);

    // Top face
    AddComponent(std::make_shared<Rectangle>(center + Vector3D({0.0, 0.0, halfHeight}),
                                             eZ,
                                             eX,
                                             width,
                                             length));
    // Bottom face
    AddComponent(std::make_shared<Rectangle>(center + Vector3D({0.0, 0.0, -halfHeight}),
                                             -1.0 * eZ,
                                             -1.0 * eX,
                                             width,
                                             length));
    // Left face
    AddComponent(std::make_shared<Rectangle>(center + Vector3D({-halfLength, 0.0, 0.0}),
                                             -1.0 * eX,
                                             -1.0 * eY,
                                             length,
                                             height));
    // Right face
    AddComponent(std::make_shared<Rectangle>(center + Vector3D({halfLength, 0.0, 0.0}),
                                             eX,
                                             eY,
                                             length,
                                             height));
    // Front face
    AddComponent(std::make_shared<Rectangle>(center + Vector3D({0.0, halfWidth, 0.0}),
                                             eY,
                                             -1.0 * eX,
                                             width,
                                             height));
    // Back face
    AddComponent(std::make_shared<Rectangle>(center + Vector3D({0.0, -halfWidth, 0.0}),
                                             -1.0 * eY,
                                             eX,
                                             width,
                                             height));
}

void BoxAxisAligned::PrintInfo() const {
    PrintInfoCompositeBase();
    std::cout << "\tLength: " << mLength << std::endl
              << "\tWidth: " << mWidth << std::endl
              << "\tHeight: " << mHeight << std::endl;
}

}  // namespace Raytracer::Geometry
