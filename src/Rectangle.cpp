#include "Scene/Rectangle.hpp"

#include "version.hpp"

namespace Raytracer {

Rectangle::Rectangle(const std::string& name, const Material& material, const Vector3D& center, const Vector3D& normal, double width, double height) :
    Object(name, material, center, normal),
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

    std::pair<double, double> uv = GetNormalizedTextureCoordinates(hitPoint);

    if (std::fabs(uv.first) <= 0.5 && std::fabs(uv.second) <= 0.5) {
        return Intersection{t, hitPoint, (denom < 0) ? mNormal : -1.0 * mNormal, &mMaterial};
    }
    return std::nullopt;
}

Color Rectangle::GetColor(const Vector3D& hitPoint) const {
    if (mTexture.has_value()) {
        auto uv = GetNormalizedTextureCoordinates(hitPoint);
        return mTexture->GetColorAt(uv.first + 0.5, uv.second + 0.5);
    } else {
        return mMaterial.GetColor();
    }
}

void Rectangle::SetTexture(std::string filename) {
    std::string filepath = TOP_LEVEL_DIR "/textures/" + filename;
    mTexture = Texture(filepath);
}

void Rectangle::PrintInfo() const {
    PrintInfoBase();
    std::cout << "Shape:\tRectangle" << std::endl
              << "\tWidth: " << mWidth << std::endl
              << "\tHeight: " << mHeight << std::endl
              << "\tColor texture:\t" << (mTexture ? "[x]" : "[ ]") << std::endl;
    mMaterial.PrintInfo();
}

std::pair<double, double> Rectangle::GetNormalizedTextureCoordinates(const Vector3D& hitPoint) const {
    Vector3D localPoint = hitPoint - mPosition;

    double uCoord = localPoint.Dot(mU);
    double vCoord = localPoint.Dot(mV);

    // Map to [0, 1]
    double u = uCoord / mWidth;
    double v = vCoord / mHeight;

    return {u, v};
}

}  // namespace Raytracer
