#include "Scene/Box.hpp"

namespace Raytracer {

Box::Box(const std::string& name, const Vector3D& center, double length, double width, double height, const std::array<Color, 6>& colors, const std::array<bool, 6>& reflectives) :
    Object(name, BLACK),
    mCenter(center),
    mLength(length),
    mWidth(width),
    mHeight(height) {
    // Top face
    Vector3D topCenter = mCenter + Vector3D({0, 0, mHeight / 2});
    Rectangle topFace("top", topCenter, Vector3D({0, 0, 1}), mLength, mWidth, colors[0]);
    topFace.SetReflective(reflectives[0]);
    mFaces.push_back(topFace);
    // Bottom face
    Vector3D bottomCenter = mCenter - Vector3D({0, 0, mHeight / 2});
    Rectangle bottomFace("bottom", bottomCenter, Vector3D({0, 0, -1}), mLength, mWidth, colors[1]);
    bottomFace.SetReflective(reflectives[1]);
    mFaces.push_back(bottomFace);
    // Front face
    Vector3D frontCenter = mCenter + Vector3D({0, mWidth / 2, 0});
    Rectangle frontFace("front", frontCenter, Vector3D({0, 1, 0}), mLength, mHeight, colors[2]);
    frontFace.SetReflective(reflectives[2]);
    mFaces.push_back(frontFace);
    // Back face
    Vector3D backCenter = mCenter - Vector3D({0, mWidth / 2, 0});
    Rectangle backFace("back", backCenter, Vector3D({0, -1, 0}), mLength, mHeight, colors[3]);
    backFace.SetReflective(reflectives[3]);
    mFaces.push_back(backFace);
    // Left face
    Vector3D leftCenter = mCenter - Vector3D({mLength / 2, 0, 0});
    Rectangle leftFace("left", leftCenter, Vector3D({-1, 0, 0}), mWidth, mHeight, colors[4]);
    leftFace.SetReflective(reflectives[4]);
    mFaces.push_back(leftFace);
    // Right face
    Vector3D rightCenter = mCenter + Vector3D({mLength / 2, 0, 0});
    Rectangle rightFace("right", rightCenter, Vector3D({1, 0, 0}), mWidth, mHeight, colors[5]);
    rightFace.SetReflective(reflectives[5]);
    mFaces.push_back(rightFace);
}

Box::Box(const std::string& name, const Vector3D& center, double length, double width, double height, const std::array<Color, 6>& colors, bool reflective) :
    Box(name, center, length, width, height, colors, std::array<bool, 6>{reflective, reflective, reflective, reflective, reflective, reflective}) {
}

Box::Box(const std::string& name, const Vector3D& center, double length, double width, double height, const Color& color, bool reflective) :
    Box(name, center, length, width, height, std::array<Color, 6>{color, color, color, color, color, color}, reflective) {
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
