#include "Geometry/Shapes/HalfTorus.hpp"

#include "Utilities/Math.hpp"

namespace Raytracer::Geometry {

HalfTorus::HalfTorus(const Vector3D& position, const Vector3D& orientation, const Vector3D& referenceDirection, double majorRadius, double minorRadius) :
    Torus(position, orientation, majorRadius, minorRadius) {
    mType = Type::HALF_TORUS;
    mOrthonormalBasis = OrthonormalBasis(orientation, referenceDirection);
}

std::optional<Intersection> HalfTorus::Intersect(const Line& line) const {
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
        Vector3D point = O + D * t;
        // Check if point is in the "half" of the torus, i.e. that phi is in [0, pi]
        double phi = std::atan2(point[1], point[0]);
        if (phi < 0.0 || phi > M_PI) {
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

double HalfTorus::SurfaceArea() const {
    return 2.0 * M_PI * M_PI * mMajorRadius * mMinorRadius;
}

std::vector<Vector3D> HalfTorus::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;

    std::uniform_real_distribution<double> dist(0.0, 1.0);

    while (points.size() < numPoints) {
        double u = dist(prng);
        double v = dist(prng);
        double w = dist(prng);

        double theta = M_PI * u;  // Half torus: theta in [0, pi]
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

std::vector<Vector3D> HalfTorus::GetKeyPoints() const {
    return {
        mPosition,
        mPosition + GetBasisVector(OrthonormalBasis::BasisVector::eX) * mMajorRadius,
        mPosition - GetBasisVector(OrthonormalBasis::BasisVector::eX) * mMajorRadius,
        mPosition + GetBasisVector(OrthonormalBasis::BasisVector::eY) * mMajorRadius};
}

std::pair<double, double> HalfTorus::GetSurfaceParameters(const Vector3D& point) const {
    return {0.0, 0.0};
}

void HalfTorus::PrintInfo() const {
    PrintInfoBase();
    std::cout << "\tReference Direction: " << mOrthonormalBasis.GetBasisVector(OrthonormalBasis::BasisVector::eX) << std::endl
              << "\tMajor Radius: " << mMajorRadius << std::endl
              << "\tMinor Radius: " << mMinorRadius << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
