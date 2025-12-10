#pragma once

#include "Geometry/Intersection.hpp"
#include "Rendering/Ray.hpp"

#include <memory>
#include <optional>
#include <string>

namespace Raytracer {

class ObjectPrimitive;

class Object {
public:
    enum class Type {
        PRIMITIVE,
        COMPOSITE
    };

    struct Intersection : public Geometry::Intersection {
        std::shared_ptr<ObjectPrimitive> object;
    };

    Object(Type type, const std::string& name);

    virtual std::optional<Intersection> Intersect(const Ray& ray) const = 0;

    std::string GetName() const;

    virtual void SetVisible(bool visible) = 0;
    virtual bool IsVisible() const = 0;

    virtual std::vector<std::shared_ptr<ObjectPrimitive>> GetLightSources() const = 0;

    // Dynamic
    virtual bool IsDynamic() const;
    virtual void Evolve(double timeDelta) = 0;

    void SetVelocity(const Vector3D& velocity);
    void SetAcceleration(const Vector3D& acceleration);
    void SetAngularVelocity(const Vector3D& angularVelocity);
    void SetSpin(const Vector3D& spin);

    virtual void PrintInfo() const = 0;

protected:
    Type mType;
    std::string mName = "";

    // Dynamic properties
    Vector3D mVelocity = Vector3D({0.0, 0.0, 0.0});
    Vector3D mAcceleration = Vector3D({0.0, 0.0, 0.0});
    Vector3D mAngularVelocity = Vector3D({0.0, 0.0, 0.0});
    Vector3D mSpin = Vector3D({0.0, 0.0, 0.0});

    virtual void Translate(const Vector3D& translation) = 0;
    virtual void Rotate(double angle, const Geometry::Line& axis = Geometry::Line()) = 0;
    virtual void Spin(double angle, const Vector3D& axis) = 0;

    void PrintInfoBase() const;
};

}  // namespace Raytracer
