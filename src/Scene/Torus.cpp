#include "Scene/Torus.hpp"

namespace Raytracer {

Torus::Torus(const std::string& name, const Material& material, const Vector3D& center, const Vector3D& normal, double innerRadius, double outerRadius) :
    Object(name, material, center, normal),
    mInnerRadius(innerRadius),
    mOuterRadius(outerRadius) {
}

int Torus::SolveQuadratic(const double coeffs[3], std::array<double, 2>& roots) {
    double a = coeffs[2];
    double b = coeffs[1];
    double c = coeffs[0];

    if (a == 0) {
        if (b == 0) {
            return 0;  // No solution
        }
        roots[0] = -c / b;
        return 1;  // Linear case
    }

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return 0;  // No real roots
    } else if (discriminant == 0) {
        roots[0] = -b / (2 * a);
        return 1;  // One real root
    } else {
        double sqrtD = std::sqrt(discriminant);
        roots[0] = (-b - sqrtD) / (2 * a);
        roots[1] = (-b + sqrtD) / (2 * a);
        return 2;  // Two real roots
    }
}

int Torus::SolveCubic(const double coeffs[4], std::array<double, 3>& roots) {
    // This function uses Cardano's method to solve the cubic equation
    // coeffs[0] + coeffs[1]*x + coeffs[2]*x^2 + coeffs[3]*x^3 = 0
    // Returns the number of real roots found and fills the roots array with them.

    double a = coeffs[3];
    double b = coeffs[2];
    double c = coeffs[1];
    double d = coeffs[0];

    if (a == 0) {
        throw std::invalid_argument("The leading coefficient cannot be zero for a cubic equation.");
    }

    // Normalize coefficients
    b /= a;
    c /= a;
    d /= a;

    // Depressed cubic substitution
    double p = c - (b * b) / 3.0;
    double q = (2.0 * b * b * b) / 27.0 - (b * c) / 3.0 + d;

    double discriminant = (q * q) / 4.0 + (p * p * p) / 27.0;
    if (discriminant > 0) {
        // One real root
        double sqrtD = std::sqrt(discriminant);
        double u = std::cbrt(-q / 2.0 + sqrtD);
        double v = std::cbrt(-q / 2.0 - sqrtD);
        roots[0] = u + v - b / 3.0;
        return 1;
    } else if (discriminant == 0) {
        // All roots real, at least two equal
        double u = std::cbrt(-q / 2.0);
        roots[0] = 2.0 * u - b / 3.0;
        roots[1] = -u - b / 3.0;
        return 2;
    } else {
        // Three distinct real roots
        double r = std::sqrt(-p * p * p / 27.0);
        double phi = std::acos(-q / (2.0 * r));
        double t = 2.0 * std::cbrt(r);
        roots[0] = t * std::cos(phi / 3.0) - b / 3.0;
        roots[1] = t * std::cos((phi + 2.0 * M_PI) / 3.0) - b / 3.0;
        roots[2] = t * std::cos((phi + 4.0 * M_PI) / 3.0) - b / 3.0;
        return 3;
    }
}

int Torus::SolveQuartic(const double coeffs[5], std::array<double, 4>& roots) {
    // This function uses Ferrari's method to solve the quartic equation
    // coeffs[0] + coeffs[1]*x + coeffs[2]*x^2 + coeffs[3]*x^3 + coeffs[4]*x^4 = 0
    // Returns the number of real roots found and fills the roots array with them.

    double a = coeffs[4];
    double b = coeffs[3];
    double c = coeffs[2];
    double d = coeffs[1];
    double e = coeffs[0];

    if (a == 0) {
        throw std::invalid_argument("The leading coefficient cannot be zero for a quartic equation.");
    }

    // Normalize coefficients
    b /= a;
    c /= a;
    d /= a;
    e /= a;

    // Depressed quartic substitution
    double p = c - (3.0 * b * b) / 8.0;
    double q = d + (b * b * b) / 8.0 - (b * c) / 2.0;
    double r = e - (3.0 * b * b * b * b) / 256.0 + (b * b * c) / 16.0 - (b * d) / 4.0;

    // Solve the resolvent cubic
    double cubicCoeffs[4] = {1, -p / 2.0, -r, (r * p - q * q / 4.0)};
    std::array<double, 3> cubicRoots;
    int numCubicRoots = SolveCubic(cubicCoeffs, cubicRoots);

    if (numCubicRoots == 0) {
        return 0;  // No real roots
    }

    double z = cubicRoots[0];  // Take one real root

    double u = std::sqrt(2.0 * z - p);
    double v = (q < 0 ? -1 : 1) * std::sqrt(z * z - r);

    // Now solve the two quadratic equations
    double quad1Coeffs[3] = {1, u, z - v};
    double quad2Coeffs[3] = {1, -u, z + v};
    std::array<double, 2> quadRoots1;
    std::array<double, 2> quadRoots2;
    int numQuadRoots1 = SolveQuadratic(quad1Coeffs, quadRoots1);
    int numQuadRoots2 = SolveQuadratic(quad2Coeffs, quadRoots2);
    int totalRoots = 0;
    for (int i = 0; i < numQuadRoots1; ++i) {
        roots[totalRoots++] = quadRoots1[i] - b / 4.0;
    }
    for (int i = 0; i < numQuadRoots2; ++i) {
        roots[totalRoots++] = quadRoots2[i] - b / 4.0;
    }
    return totalRoots;
}

std::optional<Intersection> Torus::Intersect(const Ray& ray) {
    // TODO: Needs to be fixed
    // Transform ray to torus coordinate system
    Vector3D oc = ray.GetOrigin() - mPosition;
    Vector3D d = ray.GetDirection();

    double R = mOuterRadius;
    double r = mInnerRadius;

    double ox = oc[0];
    double oy = oc[1];
    double oz = oc[2];
    double dx = d[0];
    double dy = d[1];
    double dz = d[2];

    double sum_d_sqrd = dx * dx + dy * dy + dz * dz;
    double e = ox * ox + oy * oy + oz * oz - r * r - R * R;
    double f = ox * dx + oy * dy + oz * dz;
    double four_a_sqrd = 4.0 * R * R;

    // Coefficients of the quartic equation
    double coeffs[5];
    coeffs[0] = e * e - four_a_sqrd * (r * r - oy * oy);
    coeffs[1] = 4.0 * f * e + 2.0 * four_a_sqrd * oy * dy;
    coeffs[2] = 2.0 * sum_d_sqrd * e + 4.0 * f * f + four_a_sqrd * dy * dy;
    coeffs[3] = 4.0 * sum_d_sqrd * f;
    coeffs[4] = sum_d_sqrd * sum_d_sqrd;

    // Solve the quartic equation for t
    std::array<double, 4> roots;
    int numRoots = SolveQuartic(coeffs, roots);

    // Find the smallest positive root
    double t = std::numeric_limits<double>::infinity();
    for (int i = 0; i < numRoots; ++i) {
        if (roots[i] > sEpsilon && roots[i] < t) {
            t = roots[i];
        }
    }
    if (t == std::numeric_limits<double>::infinity()) {
        return std::nullopt;  // no valid root
    }

    Vector3D hitPoint = ray.GetOrigin() + t * ray.GetDirection();

    // Compute normal at hit point
    Vector3D hitVec = hitPoint - mPosition;
    double paramSquared = R * R + r * 2;
    double sumSquared = hitVec.NormSquared();
    Vector3D normal = Vector3D({4.0 * hitVec[0] * (sumSquared - paramSquared),
                                4.0 * hitVec[1] * (sumSquared - paramSquared + 2.0 * R * R),
                                4.0 * hitVec[2] * (sumSquared - paramSquared)})
                          .Normalized();
    return Intersection{t, hitPoint, normal, this};
}

void Torus::PrintInfo() const {
    std::cout << "Torus Info:" << std::endl
              << "\tCenter: " << mPosition << std::endl
              << "\tNormal: " << mNormal << std::endl
              << "\tInner Radius: " << mInnerRadius << std::endl
              << "\tOuter Radius: " << mOuterRadius << std::endl;
    mMaterial.PrintInfo();
}

}  // namespace Raytracer
