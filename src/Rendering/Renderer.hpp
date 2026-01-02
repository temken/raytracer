#pragma once

#include "Rendering/Ray.hpp"
#include "Scene/Scene.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/Pixel.hpp"

#include <optional>
#include <random>
#include <string>

namespace Raytracer {

class Renderer {
public:
    enum class Type {
        SIMPLE,
        DETERMINISTIC,
        RAY_TRACER,
        PATH_TRACER,
        PATH_TRACER_NEE,
        // Future renderers can be added here
    };

    explicit Renderer(Type type, bool deterministic);

    virtual Pixel TraceRay(Ray ray, const Scene& scene) = 0;

    bool IsDeterministic() const;

    Type GetType() const;
    std::string GetTypeString() const;

protected:
    Type mType;
    bool mIsDeterministic = false;
    const size_t kMaximumDepth = 10;  // Maximum recursion depth for rays
    double kAmbientFactor = 0.0;

    static constexpr double kEpsilon = 1e-6;
    std::mt19937 mGenerator{std::random_device{}()};

    virtual std::optional<Object::Intersection> Intersect(const Ray& ray, const Scene& scene);

    // Overload that takes the throughput before the material interaction
    void CollectDirectLighting(Ray& ray, const Scene& scene, const Object::Intersection& intersection, const Color& throughputBefore, std::size_t numLightSamples = 0);
};

}  // namespace Raytracer
