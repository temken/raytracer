#include "Utilities/Math.hpp"

#include <cmath>
#include <complex>

namespace Raytracer {

std::vector<double> Math::SolveQuadratic(double a, double b, double c) {
    // Solve the quadratic equation at^2 + bt + c = 0 analytically.
    // Handle degenerate case: a == 0
    if (std::abs(a) < sEpsilon) {
        // Becomes linear: bt + c = 0
        if (std::abs(b) < sEpsilon) {
            return {};  // No solution or infinite solutions
        }
        return {-c / b};
    }

    double discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0.0) {
        return {};
    } else if (std::abs(discriminant) < sEpsilon) {
        return {-b / (2.0 * a)};
    } else {
        double sqrtDisc = std::sqrt(discriminant);
        return {(-b + sqrtDisc) / (2.0 * a), (-b - sqrtDisc) / (2.0 * a)};
    }
}

std::vector<double> Math::SolveCubic(double a, double b, double c, double d) {
    // Solve the cubic equation at^3 + bt^2 + ct + d = 0 analytically.

    // Handle degenerate case: a == 0
    if (std::abs(a) < sEpsilon) {
        // Becomes quadratic: bt^2 + ct + d = 0
        return SolveQuadratic(b, c, d);
    }

    // Convert to depressed cubic: t^3 + pt + q = 0
    // Substitution: x = t - b/(3a)
    double p = (3.0 * a * c - b * b) / (3.0 * a * a);
    double q = (2.0 * b * b * b - 9.0 * a * b * c + 27.0 * a * a * d) / (27.0 * a * a * a);

    // Cardano's discriminant
    double discriminant = q * q / 4.0 + p * p * p / 27.0;

    std::vector<double> roots;

    if (discriminant > sEpsilon) {
        // One real root (casus irreducibilis doesn't apply)
        double sqrtDisc = std::sqrt(discriminant);
        double u = std::cbrt(-q / 2.0 + sqrtDisc);
        double v = std::cbrt(-q / 2.0 - sqrtDisc);
        double t = u + v;
        roots.push_back(t - b / (3.0 * a));
    } else if (std::abs(discriminant) <= sEpsilon) {
        // Discriminant = 0: repeated roots
        if (std::abs(p) < sEpsilon && std::abs(q) < sEpsilon) {
            // Triple root at x = -b/(3a)
            roots.push_back(-b / (3.0 * a));
        } else {
            // One single root and one double root
            double t1 = 3.0 * q / p;
            double t2 = -3.0 * q / (2.0 * p);
            roots.push_back(t1 - b / (3.0 * a));
            roots.push_back(t2 - b / (3.0 * a));
        }
    } else {
        // Three distinct real roots (casus irreducibilis)
        // Use trigonometric method
        double rho = std::sqrt(-p * p * p / 27.0);
        double theta = std::acos(-q / (2.0 * rho));

        for (int k = 0; k < 3; ++k) {
            double t = 2.0 * std::cbrt(rho) * std::cos((theta + 2.0 * M_PI * k) / 3.0);
            roots.push_back(t - b / (3.0 * a));
        }
    }
    return roots;
}

std::vector<double> Math::SolveQuartic(double a, double b, double c, double d, double e) {
    // Solve the quartic equation at^4 + bt^3 + ct^2 + dt + e = 0 analytically.

    // Handle degenerate case: a == 0
    if (std::abs(a) < sEpsilon) {
        // Becomes cubic: bt^3 + ct^2 + dt + e = 0
        return SolveCubic(b, c, d, e);
    }

    // Special case: e == 0
    if (std::abs(e) < sEpsilon) {
        // Factor out t: t(at^3 + bt^2 + ct + d) = 0
        std::vector<double> roots = {0.0};
        // Solve the cubic equation at^3 + bt^2 + ct + d = 0
        std::vector<double> cubicRoots = SolveCubic(a, b, c, d);
        roots.insert(roots.end(), cubicRoots.begin(), cubicRoots.end());
        return roots;
    }

    // Depressed quartic u^4 + Au^2 + Bu + C = 0
    // Roots can be expressed as t = u - b/(4a)
    double A = -3.0 * b * b / (8.0 * a * a) + c / a;
    double B = b * b * b / (8.0 * a * a * a) - b * c / (2.0 * a * a) + d / a;
    double C = -3.0 * b * b * b * b / (256.0 * a * a * a * a) + b * b * c / (16.0 * a * a * a) - b * d / (4.0 * a * a) + e / a;

    std::vector<double> uRoots = SolveDepressedQuartic(A, B, C);
    std::vector<double> roots;
    for (double u : uRoots) {
        roots.push_back(u - b / (4.0 * a));
    }
    return roots;
}

std::vector<double> Math::SolveQuarticDurandKerner(double a, double b, double c, double d, double e) {
    // Solve the quartic equation at^4 + bt^3 + ct^2 + dt + e = 0 using the Durand-Kerner method.

    // Handle degenerate case: a == 0
    if (std::abs(a) < sEpsilon) {
        // Becomes cubic: bt^3 + ct^2 + dt + e = 0
        return SolveCubic(b, c, d, e);
    }

    // Normalize coefficients
    b /= a;
    c /= a;
    d /= a;
    e /= a;

    // Initial guesses for the roots (using roots of unity scaled)
    std::vector<std::complex<double>> roots = {
        std::complex<double>(1, 0),
        std::complex<double>(0, 1),
        std::complex<double>(-1, 0),
        std::complex<double>(0, -1)};

    const int maxIterations = 1000;
    const double tolerance = 1e-8;

    for (int iter = 0; iter < maxIterations; ++iter) {
        bool converged = true;
        for (size_t i = 0; i < roots.size(); ++i) {
            std::complex<double> prod(1, 0);
            for (size_t j = 0; j < roots.size(); ++j) {
                if (i != j) {
                    prod *= (roots[i] - roots[j]);
                }
            }
            std::complex<double> fValue = roots[i] * roots[i] * roots[i] * roots[i] + b * roots[i] * roots[i] * roots[i] + c * roots[i] * roots[i] + d * roots[i] + e;
            std::complex<double> delta = fValue / prod;
            roots[i] -= delta;
            if (std::abs(delta) > tolerance) {
                converged = false;
            }
        }
        if (converged) {
            break;
        }
    }

    // Extract real parts of the roots
    std::vector<double> realRoots;
    for (const auto& root : roots) {
        if (std::abs(root.imag()) < tolerance) {
            realRoots.push_back(root.real());
        }
    }

    return realRoots;
}

std::vector<double> Math::SolveBiquadratic(double a, double b, double c) {
    // Solve the biquadratic equation at^4 + bt^2 + c = 0 analytically.
    double discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0) {
        return {};
    } else if (std::abs(discriminant) < sEpsilon) {
        double z = -b / (2.0 * a);
        if (z < 0.0) {
            return {};
        } else if (std::abs(z) < sEpsilon) {
            return {0.0};
        } else {
            double sqrtZ = std::sqrt(z);
            return {sqrtZ, -sqrtZ};
        }
    }

    double zPlus = (-b + std::sqrt(discriminant)) / (2.0 * a);
    double zMinus = (-b - std::sqrt(discriminant)) / (2.0 * a);
    std::vector<double> roots;
    if (zPlus >= sEpsilon) {
        double sqrtZPlus = std::sqrt(zPlus);
        roots.push_back(sqrtZPlus);
        roots.push_back(-sqrtZPlus);
    } else if (std::abs(zPlus) < sEpsilon) {
        roots.push_back(0.0);
    }
    if (zMinus >= sEpsilon) {
        double sqrtZMinus = std::sqrt(zMinus);
        roots.push_back(sqrtZMinus);
        roots.push_back(-sqrtZMinus);
    } else if (std::abs(zMinus) < sEpsilon) {
        roots.push_back(0.0);
    }
    return roots;
}

std::vector<double> Math::SolveDepressedQuartic(double A, double B, double C) {
    if (std::abs(B) < sEpsilon) {
        // B == 0 -> biquadratic equation u^4 + Au^2 + C = 0
        return SolveBiquadratic(1.0, A, C);
    }

    // Ferrari's method: solve the cubic resolvent y^3 - Ay^2 - 4Cy + (4AC - B^2) = 0
    double cubicA = 1.0;
    double cubicB = -A;
    double cubicC = -4.0 * C;
    double cubicD = 4.0 * A * C - B * B;

    std::vector<double> yRoots = SolveCubic(cubicA, cubicB, cubicC, cubicD);
    if (yRoots.empty()) {
        return {};
    }

    // Use the first real root y of the cubic resolvent
    double y = yRoots[0];

    // Compute the discriminants for the two quadratics
    double R = std::sqrt(A / 4.0 + y);
    if (std::abs(R) < sEpsilon) {
        // R = 0 case, use alternative formulation
        double D = std::sqrt(3.0 * A / 4.0 + y * y - A * y);
        if (3.0 * A / 4.0 + y * y - A * y < 0.0) {
            return {};
        }
        double E = (4.0 * A * y - 8.0 * C - A * A) / (4.0 * D);

        // Solve u^2 + D*u + (y/2 + E/2) = 0 and u^2 - D*u + (y/2 - E/2) = 0
        std::vector<double> roots;
        double disc1 = D * D - 4.0 * (y / 2.0 + E / 2.0);
        double disc2 = D * D - 4.0 * (y / 2.0 - E / 2.0);

        if (disc1 >= 0.0) {
            double sqrt_disc1 = std::sqrt(disc1);
            roots.push_back((-D + sqrt_disc1) / 2.0);
            roots.push_back((-D - sqrt_disc1) / 2.0);
        }
        if (disc2 >= 0.0) {
            double sqrt_disc2 = std::sqrt(disc2);
            roots.push_back((D + sqrt_disc2) / 2.0);
            roots.push_back((D - sqrt_disc2) / 2.0);
        }
        return roots;
    }

    // General case: R != 0
    double D = std::sqrt(3.0 * A / 4.0 - R * R + 2.0 * y);
    double E = (4.0 * A * y - 8.0 * C - A * A) / (4.0 * R);

    std::vector<double> roots;

    // Solve u^2 + R*u + (A/4 + y/2 + E/2) = 0
    double disc1 = R * R - 4.0 * (A / 4.0 + y / 2.0 + E / 2.0);
    if (disc1 >= 0.0) {
        double sqrt_disc1 = std::sqrt(disc1);
        roots.push_back((-R + sqrt_disc1) / 2.0);
        roots.push_back((-R - sqrt_disc1) / 2.0);
    }

    // Solve u^2 - R*u + (A/4 + y/2 - E/2) = 0
    double disc2 = R * R - 4.0 * (A / 4.0 + y / 2.0 - E / 2.0);
    if (disc2 >= 0.0) {
        double sqrt_disc2 = std::sqrt(disc2);
        roots.push_back((R + sqrt_disc2) / 2.0);
        roots.push_back((R - sqrt_disc2) / 2.0);
    }

    return roots;
}

}  // namespace Raytracer
