#include "Geometry/Shapes/Box.hpp"

#include "Geometry/Shapes/Rectangle.hpp"

namespace Raytracer::Geometry {

Box::Box(const Vector3D& center, const Vector3D& heightDirection, const Vector3D& widthDirection, double length, double width, double height) :
    CompositeShape(Type::BOX, center, heightDirection, widthDirection),
    mLength(length),
    mWidth(width),
    mHeight(height) {
    ComposeShape();
}

void Box::ComposeShape() {
    mComponents.clear();
    // Create 6 rectangles for the box faces
    auto halfLength = mLength / 2.0;
    auto halfWidth = mWidth / 2.0;
    auto halfHeight = mHeight / 2.0;

    const Vector3D eX = GetBasisVector(OrthonormalBasis::BasisVector::eX);
    const Vector3D eY = GetBasisVector(OrthonormalBasis::BasisVector::eY);
    const Vector3D eZ = GetBasisVector(OrthonormalBasis::BasisVector::eZ);

    // Top face
    AddComponent(std::make_shared<Rectangle>(mPosition + halfHeight * eZ,
                                             eZ,
                                             eX,
                                             mLength,
                                             mWidth));
    // Bottom face
    AddComponent(std::make_shared<Rectangle>(mPosition - halfHeight * eZ,
                                             -1.0 * eZ,
                                             -1.0 * eX,
                                             mLength,
                                             mWidth));
    // Left face
    AddComponent(std::make_shared<Rectangle>(mPosition - halfLength * eX,
                                             -1.0 * eX,
                                             -1.0 * eY,
                                             mWidth,
                                             mHeight));
    // Right face
    AddComponent(std::make_shared<Rectangle>(mPosition + halfLength * eX,
                                             eX,
                                             eY,
                                             mWidth,
                                             mHeight));
    // Front face
    AddComponent(std::make_shared<Rectangle>(mPosition + halfWidth * eY,
                                             eY,
                                             -1.0 * eX,
                                             mLength,
                                             mHeight));
    // Back face
    AddComponent(std::make_shared<Rectangle>(mPosition - halfWidth * eY,
                                             -1.0 * eY,
                                             eX,
                                             mLength,
                                             mHeight));
}

void Box::PrintInfo() const {
    PrintInfoCompositeBase();
    std::cout << "\tLength: " << mLength << std::endl
              << "\tWidth: " << mWidth << std::endl
              << "\tHeight: " << mHeight << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
