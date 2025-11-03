#pragma once

#include "Geometry/Line.hpp"
#include "Geometry/Vector.hpp"
#include "Utilities/Color.hpp"

namespace Raytracer {

class Ray : public Geometry::Line {
public:
    Ray() = default;
    Ray(const Vector3D& origin, const Vector3D& direction);

    Color GetRadiance() const;
    void AddRadiance(const Color& contribution);

    Color GetThroughput() const;
    void UpdateThroughput(const Color& throughput);
    void UpdateThroughput(double scalar);

    size_t GetDepth() const;
    void IncrementDepth();

    double IncidentAngleCosine(const Vector3D& normal) const;
    bool IsEntering(const Vector3D& normal) const;

private:
    Color mRadiance = Color(0.0, 0.0, 0.0);    // Accumulated radiance
    Color mThroughput = Color(1.0, 1.0, 1.0);  // Monte Carlo weighting

    size_t mDepth = 0;  // Number of interactions

    static constexpr double sEpsilon = 1e-6;
};

}  // namespace Raytracer
