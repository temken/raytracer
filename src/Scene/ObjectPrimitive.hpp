#pragma once

#include "Geometry/Shape.hpp"
#include "Rendering/Material.hpp"
#include "Scene/Object.hpp"

#include <memory>
#include <optional>
#include <string>

namespace Raytracer {
class ObjectPrimitive : public Object, public std::enable_shared_from_this<ObjectPrimitive> {
public:
    ObjectPrimitive(const ::std::string& name, const Material& material, std::shared_ptr<Geometry::Shape> shape);

    virtual std::optional<Intersection> Intersect(const Ray& ray) const override;

    virtual void SetVisible(bool visible) override;
    virtual bool IsVisible() const override;

    virtual std::vector<std::shared_ptr<ObjectPrimitive>> GetLightSources() const override;

    Material& GetMaterial();
    bool EmitsLight() const;
    Color GetColor(const Intersection& intersection) const;

    std::shared_ptr<Geometry::Shape> GetShape() const;
    Vector3D GetNormal(const Intersection& intersection) const;

    virtual void Evolve(double timeDelta) override;

    virtual void PrintInfo() const override;

protected:
    friend class ObjectComposite;
    bool mVisible = true;

    Material mMaterial;
    std::shared_ptr<Geometry::Shape> mShape;

    virtual void Translate(const Vector3D& translation) override;
    virtual void Rotate(double angle, const Geometry::Line& axis = Geometry::Line()) override;
    virtual void Spin(double angle, const Vector3D& axis) override;
};

template <typename ShapeT, typename... Args>
ObjectPrimitive MakePrimitiveObject(const std::string& name, const Material& material, Args&&... args) {
    auto shape = std::make_shared<ShapeT>(std::forward<Args>(args)...);
    return ObjectPrimitive(name, material, shape);
}

}  // namespace Raytracer
