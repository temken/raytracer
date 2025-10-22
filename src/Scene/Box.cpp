#include "Scene/Box.hpp"

namespace Raytracer {

Box::Box(const std::string& name, const Material& material, const Vector3D& center, double length, double width, double height) :
    Object(name, material, center, Vector3D({0, 0, 1})),
    mLength(length),
    mWidth(width),
    mHeight(height) {
    // Top face
    Vector3D topCenter = mPosition + Vector3D({0, 0, mHeight / 2});
    Rectangle topFace("top", material, topCenter, Vector3D({0, 0, 1}), mLength, mWidth);
    mFaces.push_back(topFace);
    // Bottom face
    Vector3D bottomCenter = mPosition - Vector3D({0, 0, mHeight / 2});
    Rectangle bottomFace("bottom", material, bottomCenter, Vector3D({0, 0, -1}), mLength, mWidth);
    mFaces.push_back(bottomFace);
    // Front face
    Vector3D frontCenter = mPosition + Vector3D({0, mWidth / 2, 0});
    Rectangle frontFace("front", material, frontCenter, Vector3D({0, 1, 0}), mHeight, mLength);
    mFaces.push_back(frontFace);
    // Back face
    Vector3D backCenter = mPosition - Vector3D({0, mWidth / 2, 0});
    Rectangle backFace("back", material, backCenter, Vector3D({0, -1, 0}), mHeight, mLength);
    mFaces.push_back(backFace);
    // Left face
    Vector3D leftCenter = mPosition - Vector3D({mLength / 2, 0, 0});
    Rectangle leftFace("left", material, leftCenter, Vector3D({-1, 0, 0}), mHeight, mWidth);
    mFaces.push_back(leftFace);
    // Right face
    Vector3D rightCenter = mPosition + Vector3D({mLength / 2, 0, 0});
    Rectangle rightFace("right", material, rightCenter, Vector3D({1, 0, 0}), mHeight, mWidth);
    mFaces.push_back(rightFace);
}

double Box::GetSurfaceArea() const {
    return 2.0 * (mLength * mWidth + mWidth * mHeight + mHeight * mLength);
}

std::optional<Intersection> Box::Intersect(const Ray& ray) {
    std::optional<Intersection> closestIntersection;
    for (auto& face : mFaces) {
        auto intersection = face.Intersect(ray);
        if (intersection) {
            if (!closestIntersection || intersection->t < closestIntersection->t) {
                closestIntersection = intersection;
            }
        }
    }
    return closestIntersection;
}

void Box::PrintInfo() const {
    PrintInfoBase();
    std::cout << "Shape:\tBox" << std::endl
              << "\tDimensions: (" << mLength << "x" << mWidth << "x" << mHeight << ")" << std::endl;
    mMaterial.PrintInfo();
}

}  // namespace Raytracer
