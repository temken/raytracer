#pragma once

#include "Geometry/Shape.hpp"

#include <memory>

namespace Raytracer::Geometry {

class CompositeShape : public Shape {
public:
    CompositeShape(Type type, const Vector3D& position, const Vector3D& orientation, const Vector3D& referenceDirection = Vector3D({0.0, 0.0, 0.0}));

    void AddComponent(std::shared_ptr<Shape> component);

    virtual std::optional<Intersection> Intersect(const Line& line) const override;

    virtual double SurfaceArea() const override;
    virtual std::vector<Vector3D> SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const override;

    virtual std::vector<Vector3D> GetKeyPoints() const override;

    // Rotate around a line without changing orientation
    virtual void Rotate(double angle, const Line& axis = Line()) override;

    // Rotate around center without changing position
    virtual void Spin(double angle, Vector3D axis = Vector3D({0.0, 0.0, 0.0})) override;

    virtual void PrintInfo() const override;

protected:
    std::vector<std::shared_ptr<Shape>> mComponents;

    virtual void ComposeShape() = 0;

    void PrintInfoCompositeBase() const;
};

}  // namespace Raytracer::Geometry
