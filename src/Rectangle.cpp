#include "Rectangle.hpp"

namespace Raytracer {

Rectangle::Rectangle(const Vector3D& center, const Vector3D& normal, double width, double height, const Color& color) :
    Object(color),
    mCenter(center),
    mNormal(normal.Normalized()),
    mWidth(width),
    mHeight(height) {
    // Create two orthogonal vectors in the rectangle plane
    mU = mNormal.Cross(Vector3D({0, 1, 0}));
    if (mU.Norm() < sEpsilon) {
        mU = mNormal.Cross(Vector3D({1, 0, 0}));
    }
    mU = mU.Normalized();
    mV = mNormal.Cross(mU);
}

std::optional<Intersection> Rectangle::Intersect(const Ray& ray) const {
    double denom = mNormal.Dot(ray.GetDirection());
    if (std::fabs(denom) < sEpsilon) {
        return std::nullopt;  // Ray is parallel to the rectangle plane
    }

    double t = (mCenter - ray.GetOrigin()).Dot(mNormal) / denom;
    if (t < sEpsilon) {  // Intersection is behind the ray origin
        return std::nullopt;
    }

    Vector3D hitPoint = ray(t);
    Vector3D localPoint = hitPoint - mCenter;

    double uCoord = localPoint.Dot(mU);
    double vCoord = localPoint.Dot(mV);

    if (std::fabs(uCoord) <= 0.5 * mWidth && std::fabs(vCoord) <= 0.5 * mHeight) {
        return Intersection{t, hitPoint, (denom < 0) ? -1.0 * mNormal : mNormal, this};
    }
    return std::nullopt;
}

void Rectangle::PrintInfo() const {
    std::cout << "Rectangle: " << std::endl;
    std::cout << "  Center: " << mCenter << std::endl;
    std::cout << "  Normal: " << mNormal << std::endl;
    std::cout << "  Width: " << mWidth << std::endl;
    std::cout << "  Height: " << mHeight << std::endl;
    std::cout << "  Color: " << GetColor() << std::endl;
}

}  // namespace Raytracer
