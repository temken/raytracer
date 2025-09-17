#include "Scene/Rectangle.hpp"

#include "version.hpp"

namespace Raytracer {

Rectangle::Rectangle(const std::string& name, const Vector3D& center, const Vector3D& normal, double width, double height, const Color& color) :
    Object(name, color),
    mCenter(center),
    mNormal(normal.Normalized()),
    mWidth(width),
    mHeight(height) {
    // Create two orthogonal vectors in the rectangle plane
    mU = mNormal.Cross(Vector3D({0, 1, 0}));
    if (mU.Norm() < sEpsilon) {
        mV = mNormal.Cross(Vector3D({1, 0, 0}));
        mU = mV.Cross(mNormal);
    } else {
        mV = mNormal.Cross(mU);
    }
    mU = mU.Normalized();
    mV = mV.Normalized();
    // mU = mU.Normalized();
    // mV = mNormal.Cross(mU);
}

std::optional<Intersection> Rectangle::Intersect(const Ray& ray) {
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

    if (mTexture.has_value()) {
        double u = (uCoord / mWidth) + 0.5;
        double v = (vCoord / mHeight) + 0.5;
        Color texColor = mTexture->GetColorAt(u, v);
        SetColor(texColor);
    }

    if (std::fabs(uCoord) <= 0.5 * mWidth && std::fabs(vCoord) <= 0.5 * mHeight) {
        return Intersection{t, hitPoint, (denom < 0) ? mNormal : -1.0 * mNormal, this};
    }
    return std::nullopt;
}

void Rectangle::SetTexture(std::string filename) {
    std::string filepath = TOP_LEVEL_DIR "/textures/" + filename;
    mTexture = Texture(filepath);
}

void Rectangle::PrintInfo() const {
    std::cout << "Rectangle: " << std::endl
              << "\tCenter: " << mCenter << std::endl
              << "\tNormal: " << mNormal << std::endl
              << "\tWidth: " << mWidth << std::endl
              << "\tHeight: " << mHeight << std::endl;
    if (mTexture) {
        std::cout << "\tTexture: " << "Yes" << std::endl;
    } else {
        std::cout << "\tTexture: " << "None" << std::endl
                  << "\tColor: " << GetColor() << std::endl;
    }
}

}  // namespace Raytracer
