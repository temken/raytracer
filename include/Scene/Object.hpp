#pragma once

#include "Rendering/Ray.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/Intersection.hpp"
#include "Utilities/Vector.hpp"

#include <optional>
#include <string>

namespace Raytracer {

struct Intersection;

class Object {
public:
    explicit Object(const std::string& name, const Color& color, const Vector3D& position, const Vector3D& normal);
    virtual ~Object() = default;

    std::string GetName() const;

    void SetVisible(bool visible);
    bool IsVisible() const;

    Vector3D GetPosition() const;
    void SetPosition(const Vector3D& position);

    Vector3D GetNormal() const;
    void SetNormal(const Vector3D& normal);

    virtual Color GetColor() const;
    void SetColor(const Color& color);

    bool EmitsLight() const;
    void SetEmitsLight(bool emitsLight);

    bool IsReflective() const;
    void SetReflective(bool isReflective);

    virtual std::optional<Intersection> Intersect(const Ray& ray) = 0;

    virtual void PrintInfo() const = 0;

protected:
    std::string mName;
    bool mVisible = true;

    // Dynamic properties
    Vector3D mPosition;
    Vector3D mNormal;

    // Material properties
    Color mColor;
    bool mEmitsLight = false;
    bool mIsReflective = false;

    static constexpr double sEpsilon = 1e-6;
};

}  // namespace Raytracer
