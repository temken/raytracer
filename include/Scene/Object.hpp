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
    explicit Object(const std::string& name, const Color& color);
    virtual ~Object() = default;

    virtual Color GetColor() const;

    std::string GetName() const;

    bool EmitsLight() const;

    void SetColor(const Color& color);

    void SetEmitsLight(bool emitsLight);

    bool IsReflective() const;

    void SetReflective(bool isReflective);

    bool IsVisible() const;

    void SetVisible(bool visible);

    virtual std::optional<Intersection> Intersect(const Ray& ray) = 0;

    virtual void PrintInfo() const = 0;

protected:
    std::string mName;
    bool mVisible = true;

    // Material properties
    Color mColor;
    bool mEmitsLight = false;
    bool mIsReflective = false;

    static constexpr double sEpsilon = 1e-6;
};

}  // namespace Raytracer
