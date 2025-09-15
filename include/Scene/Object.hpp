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
    explicit Object(const std::string& name, const Color& color) :
        mName(name),
        mColor(color) {}
    virtual ~Object() = default;

    Color GetColor() const {
        return mColor;
    }

    std::string GetName() const {
        return mName;
    }

    bool EmitsLight() const {
        return mEmitsLight;
    }

    void SetEmitsLight(bool emitsLight) {
        mEmitsLight = emitsLight;
    }

    bool IsReflective() const {
        return mIsReflective;
    }

    void SetReflective(bool isReflective) {
        mIsReflective = isReflective;
    }

    bool IsVisible() const {
        return mVisible;
    }

    void SetVisible(bool visible) {
        mVisible = visible;
    }

    virtual std::optional<Intersection> Intersect(const Ray& ray) const = 0;

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
