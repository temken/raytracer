#include "Scene/Cylinder.hpp"

namespace Raytracer {

Cylinder::Cylinder(const std::string& name, const Vector3D& center, const Vector3D& normal, double radius, double height, const Color& mantleColor, const Color& capColor) :
    Object(name, mantleColor),
    mCenter(center),
    mNormal(normal.Normalized()),
    mRadius(radius),
    mHeight(height),
    mTopDisk("top_cap", center + (height / 2.0) * mNormal, mNormal, radius, capColor),
    mBottomDisk("bottom_cap", center - (height / 2.0) * mNormal, -1.0 * mNormal, radius, capColor) {
}

std::optional<Intersection> Cylinder::Intersect(const Ray& ray) const {
    auto mantleIntersection = IntersectMantle(ray);
    auto capIntersection = IntersectCaps(ray);

    std::optional<Intersection> closestIntersection;
    if (mantleIntersection && mantleIntersection->t > sEpsilon) {
        closestIntersection = mantleIntersection;
    }
    if (capIntersection && capIntersection->t > sEpsilon) {
        if (!closestIntersection || capIntersection->t < closestIntersection->t) {
            closestIntersection = capIntersection;
        }
    }
    return closestIntersection;
}

void Cylinder::PrintInfo() const {
    std::cout << "Cylinder Info:" << std::endl
              << "\tCenter: " << mCenter << std::endl
              << "\tNormal: " << mNormal << std::endl
              << "\tRadius: " << mRadius << std::endl
              << "\tHeight: " << mHeight << std::endl
              << "\tColor (Mantle): " << GetColor() << std::endl
              << "\tColor (Caps): " << mTopDisk.GetColor() << std::endl;
}

std::optional<Intersection> Cylinder::IntersectCaps(const Ray& ray) const {
    auto topIntersection = mTopDisk.Intersect(ray);
    auto bottomIntersection = mBottomDisk.Intersect(ray);

    std::optional<Intersection> closestIntersection;
    if (topIntersection && topIntersection->t > sEpsilon) {
        closestIntersection = topIntersection;
    }
    if (bottomIntersection && bottomIntersection->t > sEpsilon) {
        if (!closestIntersection || bottomIntersection->t < closestIntersection->t) {
            closestIntersection = bottomIntersection;
        }
    }
    return closestIntersection;
}

std::optional<Intersection> Cylinder::IntersectMantle(const Ray& ray) const {
    Vector3D d = ray.GetDirection();
    Vector3D oc = ray.GetOrigin() - mCenter;

    double dDotN = d.Dot(mNormal);
    Vector3D dPerp = d - dDotN * mNormal;

    double ocDotN = oc.Dot(mNormal);
    Vector3D ocPerp = oc - ocDotN * mNormal;

    double a = dPerp.NormSquared();
    double b = 2.0 * ocPerp.Dot(dPerp);
    double c = ocPerp.NormSquared() - mRadius * mRadius;

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0.0) {
        return std::nullopt;
    }

    double sqrtD = std::sqrt(discriminant);
    std::array<double, 2> roots = {
        (-b - sqrtD) / (2.0 * a),
        (-b + sqrtD) / (2.0 * a)};

    double tCylinder = std::numeric_limits<double>::infinity();
    for (double r : roots) {
        if (r > sEpsilon && r < tCylinder) {
            tCylinder = r;
        }
    }
    if (tCylinder == std::numeric_limits<double>::infinity()) {
        return std::nullopt;  // No valid root
    }

    // Check if the intersection point is within the height bounds
    Vector3D hitPoint = ray(tCylinder);
    double heightAtHit = (hitPoint - mCenter).Dot(mNormal);
    if (std::fabs(heightAtHit) <= mHeight / 2.0) {
        Vector3D normalAtHit = (hitPoint - mCenter - heightAtHit * mNormal).Normalized();
        return Intersection{tCylinder, hitPoint, normalAtHit, this};
    }

    return std::nullopt;
}

}  // namespace Raytracer
