#pragma once

#include "Geometry/OrthonormalBasis.hpp"
#include "Geometry/Vector.hpp"
#include "Rendering/Ray.hpp"
#include "Scene/Object.hpp"
#include "Scene/ObjectPrimitive.hpp"

namespace Raytracer {

class ObjectComposite : public Object {
public:
    ObjectComposite(const std::string& name, double referenceLength, const Vector3D& position, const Vector3D& orientation, const Vector3D& referenceDirection = Vector3D({0.0, 0.0, 0.0}));

    std::size_t NumberOfComponents() const;
    void AddComponent(const std::shared_ptr<ObjectPrimitive>& component);

    virtual std::optional<Intersection> Intersect(const Ray& ray) const override;

    virtual void SetVisible(bool visible) override;
    virtual bool IsVisible() const override;

    virtual std::vector<std::shared_ptr<ObjectPrimitive>> GetLightSources() const override;

    virtual bool IsDynamic() const override;
    virtual void Evolve(double timeDelta) override;

    virtual void PrintInfo() const override;

protected:
    double mReferenceLength;
    Vector3D mPosition;
    Geometry::OrthonormalBasis mOrthonormalBasis;

    std::vector<std::shared_ptr<ObjectPrimitive>> mComponents;
    std::vector<std::shared_ptr<ObjectPrimitive>> mLightSources;

    virtual void Translate(const Vector3D& translation) override;
    virtual void Rotate(double angle, const Geometry::Line& axis = Geometry::Line()) override;
    virtual void Spin(double angle, const Vector3D& axis) override;
};

}  // namespace Raytracer
