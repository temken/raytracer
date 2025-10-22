#pragma once

#include "Rendering/Material.hpp"
#include "Rendering/Ray.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/Intersection.hpp"
#include "Utilities/Vector.hpp"

#include <optional>
#include <random>
#include <string>
#include <vector>

namespace Raytracer {

struct Intersection;

class Object {
public:
    explicit Object(const std::string& name, const Material& material, const Vector3D& position, const Vector3D& normal);
    virtual ~Object() = default;

    std::string GetName() const;

    void SetVisible(bool visible);
    bool IsVisible() const;

    Material& GetMaterial();
    void SetMaterial(const Material& material);

    Vector3D GetPosition() const;
    void SetPosition(const Vector3D& position);

    Vector3D GetNormal() const;
    void SetNormal(const Vector3D& normal);

    Vector3D GetVelocity() const;
    void SetVelocity(const Vector3D& velocity);

    Vector3D GetAngularVelocity() const;
    void SetAngularVelocity(const Vector3D& angularVelocity);

    Vector3D GetSpin() const;
    void SetSpin(const Vector3D& spin);

    Color GetColor(const Intersection& intersection) const;
    bool EmitsLight() const;

    virtual double GetSurfaceArea() const = 0;

    virtual std::optional<Intersection> Intersect(const Ray& ray) = 0;

    // Relative to center, range [-0.5, 0.5]
    virtual std::pair<double, double> GetTextureCoordinates(const Vector3D& hitPoint) const;

    void Evolve(double timeStep);

    virtual void PrintInfo() const;

protected:
    std::string mName;
    bool mVisible = true;

    Material mMaterial;

    // Dynamic properties
    Vector3D mPosition;
    Vector3D mNormal;

    Vector3D mVelocity = Vector3D({0.0, 0.0, 0.0});
    Vector3D mAngularVelocity = Vector3D({0.0, 0.0, 0.0});
    Vector3D mSpin = Vector3D({0.0, 0.0, 0.0});

    static constexpr double sEpsilon = 1e-6;

    virtual void Translate(const Vector3D& translation);
    virtual void Rotate(double angle, const Vector3D& axis);
    virtual void Spin(double angle, const Vector3D& axis);

    void PrintInfoBase() const;
};

}  // namespace Raytracer
