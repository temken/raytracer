#pragma once

#include "Geometry/Intersection.hpp"
#include "Geometry/Line.hpp"
#include "Geometry/Shapes/Ring.hpp"
#include "Geometry/Vector.hpp"

#include <optional>
#include <random>

namespace Raytracer::Geometry {

class Disk : public Ring {
public:
    Disk(const Vector3D& position, const Vector3D& normal, double radius);

    virtual std::vector<Vector3D> GetKeyPoints() const override;

    virtual void PrintInfo() const override;

private:
};

}  // namespace Raytracer::Geometry
