#pragma once

#include "Geometry/Intersection.hpp"
#include "Geometry/Line.hpp"
#include "Geometry/OrthonormalBasis.hpp"
#include "Geometry/Vector.hpp"

#include <optional>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace Raytracer::Geometry {

class Shape {
public:
    enum class Type {
        BASE,
        BOX,
        BOX_AXIS_ALIGNED,
        CYLINDER,
        DISK,
        PLANE,
        RECTANGLE,
        SPHERE,
        TUBE,
    };

    // Constructor
    // Orientation defines the local Z axis
    Shape(Type type, const Vector3D& position, const Vector3D& orientation, const Vector3D& referenceDirection = Vector3D({0.0, 0.0, 0.0}));
    virtual ~Shape() = default;

    virtual std::optional<Intersection> Intersect(const Line& line) const = 0;

    Type GetType() const;
    Vector3D GetBasisVector(OrthonormalBasis::BasisVector axis) const;
    Vector3D GetOrientation() const;

    virtual double SurfaceArea() const = 0;

    virtual std::vector<Vector3D> SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const = 0;
    virtual std::vector<Vector3D> GetKeyPoints() const = 0;

    // Parametrize the surface in range [-0.5, 0.5]
    virtual std::pair<double, double> GetSurfaceParameters(const Vector3D& point) const;

    virtual void PrintInfo() const;

protected:
    Type mType;
    Vector3D mPosition;
    OrthonormalBasis mOrthonormalBasis;

    static constexpr double sEpsilon = 1e-6;

    void Rotate(double angle, const Vector3D& axis);
    void Spin(double angle, OrthonormalBasis::BasisVector axis = OrthonormalBasis::BasisVector::eZ);

    static std::string TypeToString(Type type);

    void PrintInfoBase() const;
};

}  // namespace Raytracer::Geometry
