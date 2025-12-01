#pragma once

#include <vector>

namespace Raytracer {

class Math {
public:
    // Solve the quadratic equation at^2 + bt + c = 0
    static std::vector<double> SolveQuadratic(double a, double b, double c);

    // Solve the cubic equation at^3 + bt^2 + ct + d = 0
    static std::vector<double> SolveCubic(double a, double b, double c, double d);

    // Solve the quartic equation at^4 + bt^3 + ct^2 + dt + e = 0
    static std::vector<double> SolveQuartic(double a, double b, double c, double d, double e);
    static std::vector<double> SolveQuarticDurandKerner(double a, double b, double c, double d, double e);

private:
    static constexpr double sEpsilon = 1e-6;

    // Solve the biquadratic equation at^4 + bt^2 + c = 0
    static std::vector<double> SolveBiquadratic(double a, double b, double c);

    // Solve the depressed quartic u^4 + Au^2 + Bu + C = 0
    static std::vector<double> SolveDepressedQuartic(double A, double B, double C);
};

}  // namespace Raytracer
