#pragma once

#include "Scene/Object.hpp"

#include <memory>

namespace Raytracer {

class CompositeObject : public Object {
public:
    CompositeObject(const std::string& name, const Material& material, const Vector3D& position, const Vector3D& normal);

    void AddComponent(std::shared_ptr<Object> component);
    const std::vector<std::shared_ptr<Object>>& GetComponents() const;

    virtual double GetSurfaceArea() const override final;
    virtual std::vector<Vector3D> SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const override final;
    virtual std::vector<Vector3D> GetKeyPoints() const override final;

    virtual std::optional<Intersection> Intersect(const Ray& ray) override final;

protected:
    std::vector<std::shared_ptr<Object>> mComponents;

    void PrintCompositeInfo() const;
};

}  // namespace Raytracer
