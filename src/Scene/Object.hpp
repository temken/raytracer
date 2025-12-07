#pragma once

#include "Geometry/Line.hpp"
#include "Geometry/Shape.hpp"
#include "Geometry/Vector.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Ray.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/HitRecord.hpp"

#include <memory>
#include <optional>
#include <random>
#include <string>
#include <vector>

namespace Raytracer {

class Object : public std::enable_shared_from_this<Object> {
public:
    explicit Object(const std::string& name, const Material& material, std::shared_ptr<Geometry::Shape> shape);
    ~Object() = default;

    std::string GetName() const;

    void SetVisible(bool visible);
    bool IsVisible() const;

    Material& GetMaterial();
    void SetMaterial(const Material& material);

    std::shared_ptr<Geometry::Shape> GetShape() const;
    void SetShape(const std::shared_ptr<Geometry::Shape>& shape);

    // Static
    Color GetColor(const HitRecord& hitRecord) const;
    Vector3D GetNormal(const HitRecord& hitRecord) const;
    bool EmitsLight() const;

    // Dynamic
    Vector3D GetVelocity() const;
    void SetVelocity(const Vector3D& velocity);

    Vector3D GetAcceleration() const;
    void SetAcceleration(const Vector3D& acceleration);

    Vector3D GetAngularVelocity() const;
    void SetAngularVelocity(const Vector3D& angularVelocity);

    Vector3D GetSpin() const;
    void SetSpin(const Vector3D& spin);

    std::optional<HitRecord> Intersect(const Ray& ray);

    bool IsDynamic() const;
    void Evolve(double timeStep);

    void PrintInfo() const;

protected:
    std::string mName;
    bool mVisible = true;

    Material mMaterial;
    std::shared_ptr<Geometry::Shape> mShape;

    // Dynamic properties
    Vector3D mVelocity = Vector3D({0.0, 0.0, 0.0});
    Vector3D mAcceleration = Vector3D({0.0, 0.0, 0.0});
    Vector3D mAngularVelocity = Vector3D({0.0, 0.0, 0.0});
    Vector3D mSpin = Vector3D({0.0, 0.0, 0.0});

    static constexpr double sEpsilon = 1e-6;

    void Translate(const Vector3D& translation);
    void Rotate(double angle, const Geometry::Line& axis = Geometry::Line());
    void Spin(double angle, const Vector3D& axis);

    void PrintInfoBase() const;
};

template <typename ShapeT, typename... Args>
Object MakeObject(const std::string& name, const Material& material, Args&&... args) {
    auto shape = std::make_shared<ShapeT>(std::forward<Args>(args)...);
    return Object(name, material, shape);
}

}  // namespace Raytracer
