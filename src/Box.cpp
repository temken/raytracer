#include "Scene/Box.hpp"

namespace Raytracer {

Box::Box(const std::string& name, const Vector3D& center, double length, double width, double height, const std::array<Color, 6>& colors) :
    Object(name, BLACK),
    mCenter(center),
    mLength(length),
    mWidth(width),
    mHeight(height) {
    // Top face
    Vector3D topCenter = mCenter + Vector3D({0, 0, mHeight / 2});
    mFaces.push_back(Rectangle("top", topCenter, Vector3D({0, 0, 1}), mLength, mWidth, colors[0]));
    // Bottom face
    Vector3D bottomCenter = mCenter - Vector3D({0, 0, mHeight / 2});
    mFaces.push_back(Rectangle("bottom", bottomCenter, Vector3D({0, 0, -1}), mLength, mWidth, colors[1]));
    // Front face
    Vector3D frontCenter = mCenter + Vector3D({0, mWidth / 2, 0});
    mFaces.push_back(Rectangle("front", frontCenter, Vector3D({0, 1, 0}), mLength, mHeight, colors[2]));
    // Back face
    Vector3D backCenter = mCenter - Vector3D({0, mWidth / 2, 0});
    mFaces.push_back(Rectangle("back", backCenter, Vector3D({0, -1, 0}), mLength, mHeight, colors[3]));
    // Left face
    Vector3D leftCenter = mCenter - Vector3D({mLength / 2, 0, 0});
    mFaces.push_back(Rectangle("left", leftCenter, Vector3D({-1, 0, 0}), mWidth, mHeight, colors[4]));
    // Right face
    Vector3D rightCenter = mCenter + Vector3D({mLength / 2, 0, 0});
    mFaces.push_back(Rectangle("right", rightCenter, Vector3D({1, 0, 0}), mWidth, mHeight, colors[5]));
}

Box::Box(const std::string& name, const Vector3D& center, double length, double width, double height, const Color& color) :
    Box(name, center, length, width, height, std::array<Color, 6>{color, color, color, color, color, color}) {
}

Box::Box(const std::string& name, const Vector3D& center, double size, const Color& color) :
    Box(name, center, size, size, size, color) {
}

Box::Box(const std::string& name, const Vector3D& center, double size, const std::array<Color, 6>& colors) :
    Box(name, center, size, size, size, colors) {
}

std::optional<Intersection> Box::Intersect(const Ray& ray) const {
    std::optional<Intersection> closestIntersection;
    for (const auto& face : mFaces) {
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
    std::cout << "Box: " << std::endl;
    std::cout << "\tCenter: " << mCenter << std::endl;
    std::cout << "\tDimensions: (" << mLength << "x" << mWidth << "x" << mHeight << ")" << std::endl;
    for (size_t i = 0; i < mFaces.size(); ++i) {
        std::cout << "\tFace " << i + 1 << ":" << std::endl
                  << "\t";
        mFaces[i].PrintInfo();
    }
}

}  // namespace Raytracer
