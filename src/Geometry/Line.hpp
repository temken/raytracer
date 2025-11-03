#pragma once

#include "Geometry/Vector.hpp"

#include <limits>
#include <ostream>

namespace Raytracer::Geometry {

class Line {
public:
    Line(const Vector3D& origin, const Vector3D& direction, double tMin = -std::numeric_limits<double>::infinity());

    Vector3D GetOrigin() const;
    Vector3D GetDirection() const;
    double GetTMin() const;

    void SetOrigin(const Vector3D& newOrigin);
    void SetDirection(const Vector3D& newDirection);

    Vector3D PointAtParameter(double t) const;

    // Operators
    Vector3D operator()(double t) const;
    bool operator==(const Line& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Line& line);

protected:
    Vector3D mOrigin;
    Vector3D mDirection;
    const double tMin;
};

}  // namespace Raytracer::Geometry
