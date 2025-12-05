#include "Geometry/Shapes/Torus.hpp"

#include "Utilities/Math.hpp"

namespace Raytracer::Geometry {

Torus::Torus(const Vector3D& position, const Vector3D& orientation, double majorRadius, double minorRadius) :
    Shape(Shape::Type::TORUS, position, orientation),
    mMajorRadius(majorRadius),
    mMinorRadius(minorRadius) {}

std::optional<Intersection> Torus::Intersect(const Line& line) const {
    // Line in world space
    Vector3D origin = line.GetOrigin();
    Vector3D direction = line.GetDirection();

    // Transform line to local torus coordinates (torus centered at origin, major circle in XY plane)
    Vector3D O = mOrthonormalBasis.ToLocal(origin - mPosition);  // local origin
    Vector3D D = mOrthonormalBasis.ToLocal(direction);           // local direction (NOT normalized on purpose)

    auto quarticCoeffs = ComputeQuarticCoefficients(O, D);
    std::vector<double> roots = Math::SolveQuarticDurandKerner(quarticCoeffs[0], quarticCoeffs[1], quarticCoeffs[2], quarticCoeffs[3], quarticCoeffs[4]);

    // Filter roots: keep smallest t within [tMin + eps, tMax - eps]
    double tMin = std::numeric_limits<double>::infinity();
    for (double t : roots) {
        if (!std::isfinite(t)) {
            continue;
        }
        if (t < line.GetTMin() + sEpsilon) {
            continue;
        }
        if (t < tMin) {
            tMin = t;
        }
    }

    if (tMin == std::numeric_limits<double>::infinity()) {
        return std::nullopt;
    }

    // Compute local and global intersection point
    Vector3D localPoint = O + D * tMin;
    Vector3D globalPoint = mOrthonormalBasis.ToGlobal(localPoint) + mPosition;

    // Transform normal to global space (assumes ToGlobal preserves directions, not positions)
    Vector3D normal = ComputeNormalAtPoint(localPoint);

    return Intersection{tMin, globalPoint, normal};
}

double Torus::SurfaceArea() const {
    return 4.0 * M_PI * M_PI * mMajorRadius * mMinorRadius;
}

std::vector<Vector3D> Torus::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;

    std::uniform_real_distribution<double> dist(0.0, 1.0);

    while (points.size() < numPoints) {
        double u = dist(prng);
        double v = dist(prng);
        double w = dist(prng);

        double theta = 2.0 * M_PI * u;
        double phi = 2.0 * M_PI * v;

        double W = (mMajorRadius + mMinorRadius * std::cos(phi)) / (mMajorRadius + mMinorRadius);

        if (w > W) {
            continue;
        }

        double x = (mMajorRadius + mMinorRadius * std::cos(phi)) * std::cos(theta);
        double y = (mMajorRadius + mMinorRadius * std::cos(phi)) * std::sin(theta);
        double z = mMinorRadius * std::sin(phi);

        Vector3D localPoint{x, y, z};
        Vector3D globalPoint = mOrthonormalBasis.ToGlobal(localPoint) + mPosition;
        points.push_back(globalPoint);
    }
    return points;
}

std::vector<Vector3D> Torus::GetKeyPoints() const {
    return {mPosition,
            mPosition + GetBasisVector(OrthonormalBasis::BasisVector::eX) * mMajorRadius,
            mPosition - GetBasisVector(OrthonormalBasis::BasisVector::eX) * mMajorRadius,
            mPosition + GetBasisVector(OrthonormalBasis::BasisVector::eY) * mMajorRadius,
            mPosition - GetBasisVector(OrthonormalBasis::BasisVector::eY) * mMajorRadius};
}

std::pair<double, double> Torus::GetSurfaceParameters(const Vector3D& point) const {
    return {0.0, 0.0};
}

void Torus::PrintInfo() const {
    PrintInfoBase();
    std::cout << "\tMajor Radius: " << mMajorRadius << std::endl
              << "\tMinor Radius: " << mMinorRadius << std::endl
              << std::endl;
}

std::array<double, 5> Torus::ComputeQuarticCoefficients(const Vector3D& localOrigin, const Vector3D& localDirection) const {
    double R = mMajorRadius;
    double r = mMinorRadius;

    // Torus-ray intersection: F(t) = (|O+tD|² + R² - r²)² - 4R²((Ox+tDx)² + (Oy+tDy)²) = 0
    double sum_O = localOrigin.Dot(localOrigin);                                                     // |O|²
    double sum_D = localDirection.Dot(localDirection);                                               // |D|²
    double dot_OD = localOrigin.Dot(localDirection);                                                 // O·D
    double sum_Oxy = localOrigin[0] * localOrigin[0] + localOrigin[1] * localOrigin[1];              // Ox² + Oy²
    double sum_Dxy = localDirection[0] * localDirection[0] + localDirection[1] * localDirection[1];  // Dx² + Dy²
    double dot_ODxy = localOrigin[0] * localDirection[0] + localOrigin[1] * localDirection[1];       // Ox*Dx + Oy*Dy

    double G0 = sum_O + R * R - r * r;  // G(0) = |O|² + R² - r²
    double G1 = 2.0 * dot_OD;           // G'(0) = 2(O·D)
    double G2 = sum_D;                  // G''(0)/2 = |D|²

    double H0 = sum_Oxy;         // H(0) = Ox² + Oy²
    double H1 = 2.0 * dot_ODxy;  // H'(0) = 2(Ox*Dx + Oy*Dy)
    double H2 = sum_Dxy;         // H''(0)/2 = Dx² + Dy²

    // Quartic F(t) = G(t)² - 4R²H(t): at⁴ + bt³ + ct² + dt + e = 0
    double a = G2 * G2;                                     // t⁴: G2²
    double b = 2.0 * G1 * G2;                               // t³: 2*G1*G2
    double c = G1 * G1 + 2.0 * G0 * G2 - 4.0 * R * R * H2;  // t²: G1² + 2*G0*G2 - 4R²*H2
    double d = 2.0 * G0 * G1 - 4.0 * R * R * H1;            // t¹: 2*G0*G1 - 4R²*H1
    double e = G0 * G0 - 4.0 * R * R * H0;                  // t⁰: G0² - 4R²*H0

    return {a, b, c, d, e};
}

Vector3D Torus::ComputeNormalAtPoint(const Vector3D& localPoint) const {
    // Compute gradient of F(X) = (|X|^2 + R^2 - r^2)^2 - 4 R^2 (x^2 + y^2)
    // ∇F = (4 x Qp - 8 R^2 x, 4 y Qp - 8 R^2 y, 4 z Qp) where Qp = |X|^2 + R^2 - r^2
    double R = mMajorRadius;
    double r = mMinorRadius;
    double Qp = localPoint.Dot(localPoint) + R * R - r * r;
    Vector3D localNormal{
        4.0 * localPoint[0] * Qp - 8.0 * R * R * localPoint[0],
        4.0 * localPoint[1] * Qp - 8.0 * R * R * localPoint[1],
        4.0 * localPoint[2] * Qp};
    localNormal.Normalize();

    // Transform normal to global space (assumes ToGlobal preserves directions, not positions)
    Vector3D globalNormal = mOrthonormalBasis.ToGlobal(localNormal);
    globalNormal.Normalize();
    return globalNormal;
}

}  // namespace Raytracer::Geometry
