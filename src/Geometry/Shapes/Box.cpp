#include "Geometry/Shapes/Box.hpp"

#include "Geometry/Shapes/Rectangle.hpp"

namespace Raytracer::Geometry {

Box::Box(const Vector3D& center, const Vector3D& heightDirection, const Vector3D& widthDirection, double length, double width, double height) :
    CompositeShape(Type::BOX, center, heightDirection, widthDirection),
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
                                             length,
                                             width));
    // Bottom face
    AddComponent(std::make_shared<Rectangle>(center + Vector3D({0.0, 0.0, -halfHeight}),
                                             -1.0 * eZ,
                                             -1.0 * eX,
                                             length,
                                             width));
    // Left face
    AddComponent(std::make_shared<Rectangle>(center + Vector3D({-halfLength, 0.0, 0.0}),
                                             -1.0 * eX,
                                             -1.0 * eY,
                                             width,
                                             height));
    // Right face
    AddComponent(std::make_shared<Rectangle>(center + Vector3D({halfLength, 0.0, 0.0}),
                                             eX,
                                             eY,
                                             width,
                                             height));
    // Front face
    AddComponent(std::make_shared<Rectangle>(center + Vector3D({0.0, halfWidth, 0.0}),
                                             eY,
                                             -1.0 * eX,
                                             length,
                                             height));
    // Back face
    AddComponent(std::make_shared<Rectangle>(center + Vector3D({0.0, -halfWidth, 0.0}),
                                             -1.0 * eY,
                                             eX,
                                             length,
                                             height));
}

void Box::PrintInfo() const {
    PrintInfoCompositeBase();
    std::cout << "\tLength: " << mLength << std::endl
              << "\tWidth: " << mWidth << std::endl
              << "\tHeight: " << mHeight << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
