#include "Scene/Rectangle.hpp"

#include "Version.hpp"

namespace Raytracer {

Rectangle::Rectangle(const std::string& name, const Material& material, const Vector3D& center, const Vector3D& normal, double width, double height) :
    Object(name, material, center, normal),
    mWidth(width),
    mHeight(height) {
    // Create two orthogonal vectors in the rectangle plane
    mU = mNormal.Cross(Vector3D({0, 1, 0}));
    if (mU.Norm() < sEpsilon) {
        mU = mNormal.Cross(Vector3D({1, 0, 0}));
        mV = mU.Cross(mNormal);
    } else {
        mV = mNormal.Cross(mU);
    }
    mU = mU.Normalized();
    mV = mV.Normalized();
}

double Rectangle::GetSurfaceArea() const {
    return mWidth * mHeight;
}

std::optional<Intersection> Rectangle::Intersect(const Ray& ray) {
    double denom = mNormal.Dot(ray.GetDirection());
    if (std::fabs(denom) < sEpsilon) {
        return std::nullopt;  // Ray is parallel to the rectangle plane
    }

    double t = (mPosition - ray.GetOrigin()).Dot(mNormal) / denom;
    if (t < sEpsilon) {  // Intersection is behind the ray origin
        return std::nullopt;
    }

    Vector3D hitPoint = ray(t);

    std::pair<double, double> uv = GetTextureCoordinates(hitPoint);

    if (std::fabs(uv.first) <= 0.5 && std::fabs(uv.second) <= 0.5) {
        return Intersection{t, hitPoint, mNormal, this};
    }
    return std::nullopt;
}

std::pair<double, double> Rectangle::GetTextureCoordinates(const Vector3D& hitPoint) const {
    // Relative to center
    Vector3D localPoint = hitPoint - mPosition;

    double uCoord = localPoint.Dot(mU);
    double vCoord = localPoint.Dot(mV);

    // Map to [-0.5, 0.5]
    double u = uCoord / mWidth;
    double v = vCoord / mHeight;

    return {u, v};
}

void Rectangle::PrintInfo() const {
    PrintInfoBase();
    std::cout << "Shape:\tRectangle" << std::endl
              << "\tWidth: " << mWidth << std::endl
              << "\tHeight: " << mHeight << std::endl;
    mMaterial.PrintInfo();
}

}  // namespace Raytracer
