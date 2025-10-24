#include "Scene/Box.hpp"

namespace Raytracer {

Box::Box(const std::string& name, const Material& material, const Vector3D& center, double length, double width, double height) :
    CompositeObject(name, material, center, Vector3D({0, 0, 1})),
    mLength(length),
    mWidth(width),
    mHeight(height) {
    // Create and add 6 rectangle faces

    // Front face
    Vector3D frontCenter = mPosition + Vector3D({0, mWidth / 2, 0});
    auto frontFace = std::make_shared<Rectangle>("front", material, frontCenter, Vector3D({0, 1, 0}), mHeight, mLength);
    AddComponent(frontFace);

    // Back face
    Vector3D backCenter = mPosition - Vector3D({0, mWidth / 2, 0});
    auto backFace = std::make_shared<Rectangle>("back", material, backCenter, Vector3D({0, -1, 0}), mHeight, mLength);
    AddComponent(backFace);

    // Top face
    Vector3D topCenter = mPosition + Vector3D({0, 0, mHeight / 2});
    auto topFace = std::make_shared<Rectangle>("top", material, topCenter, Vector3D({0, 0, 1}), mLength, mWidth);
    AddComponent(topFace);

    // Bottom face
    Vector3D bottomCenter = mPosition - Vector3D({0, 0, mHeight / 2});
    auto bottomFace = std::make_shared<Rectangle>("bottom", material, bottomCenter, Vector3D({0, 0, -1}), mLength, mWidth);
    AddComponent(bottomFace);

    // Left face
    Vector3D leftCenter = mPosition - Vector3D({mLength / 2, 0, 0});
    auto leftFace = std::make_shared<Rectangle>("left", material, leftCenter, Vector3D({-1, 0, 0}), mHeight, mWidth);
    AddComponent(leftFace);

    // Right face
    Vector3D rightCenter = mPosition + Vector3D({mLength / 2, 0, 0});
    auto rightFace = std::make_shared<Rectangle>("right", material, rightCenter, Vector3D({1, 0, 0}), mHeight, mWidth);
    AddComponent(rightFace);
}

void Box::PrintInfo() const {
    PrintCompositeInfo();
    std::cout << "Shape:\tBox" << std::endl
              << "\tDimensions: (" << mLength << "x" << mWidth << "x" << mHeight << ")" << std::endl;
    mMaterial.PrintInfo();
}

}  // namespace Raytracer
