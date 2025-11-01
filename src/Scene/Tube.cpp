#include "Scene/Tube.hpp"

namespace Raytracer {

Tube::Tube(const std::string& name, const Material& material, const Vector3D& center, const Vector3D& normal, double radius, double length) :
    Object(name, material, center, normal),
    mRadius(radius),
    mLength(length) {
}

double Tube::GetSurfaceArea() const {
    return 2 * M_PI * mRadius * mLength;
}

std::vector<Vector3D> Tube::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;

    // Distributions for uniform sampling
    std::uniform_real_distribution<double> angleDist(0.0, 2.0 * M_PI);
    std::uniform_real_distribution<double> lengthDist(-0.5 * mLength, 0.5 * mLength);

    // Build an orthonormal basis around the cylinder axis
    Vector3D w = mNormal.Normalized();  // axis direction
    // Pick a reference vector that is *not* parallel to w
    Vector3D reference = (std::fabs(w[0]) > 0.9) ? Vector3D({0, 1, 0}) : Vector3D({1, 0, 0});
    // First perpendicular vector
    Vector3D u = w.Cross(reference).Normalized();
    // Second perpendicular vector (completes the right-handed system)
    Vector3D v = w.Cross(u);

    for (std::size_t i = 0; i < numPoints; ++i) {
        double theta = angleDist(prng);
        double h = lengthDist(prng);

        // Point on circle of given height
        Vector3D circlePoint = mPosition + w * h + mRadius * (std::cos(theta) * u + std::sin(theta) * v);
        points.push_back(circlePoint);
    }

    return points;
}
std::vector<Vector3D> Tube::GetKeyPoints() const {
    std::vector<Vector3D> keyPoints = {
        mPosition,
        mPosition + mLength / 4.0 * mNormal,
        mPosition - mLength / 4.0 * mNormal,
        mPosition + (mLength / 2.0) * mNormal,
        mPosition - (mLength / 2.0) * mNormal,
    };
    return keyPoints;
}

std::optional<Intersection> Tube::Intersect(const Ray& ray) {
    Vector3D d = ray.GetDirection();
    Vector3D oc = ray.GetOrigin() - mPosition;

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
    double heightAtHit = (hitPoint - mPosition).Dot(mNormal);
    if (std::fabs(heightAtHit) <= mLength / 2.0) {
        Vector3D normalAtHit = (hitPoint - mPosition - heightAtHit * mNormal).Normalized();
        return Intersection{tCylinder, hitPoint, normalAtHit, this};
    }

    return std::nullopt;
}

void Tube::PrintInfo() const {
    PrintInfoBase();
    std::cout << "Shape:\tTube" << std::endl
              << "\tRadius: " << mRadius << std::endl
              << "\tLength: " << mLength << std::endl;
    mMaterial.PrintInfo();
}

}  // namespace Raytracer
