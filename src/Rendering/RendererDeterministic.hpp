#pragma once

#include "Rendering/Ray.hpp"
#include "Rendering/Renderer.hpp"
#include "Scene/Scene.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/Intersection.hpp"

namespace Raytracer {

class RendererDeterministic : public Renderer {
public:
    RendererDeterministic() :
        Renderer(Type::DETERMINISTIC, true) {};
    virtual Color TraceRay(Ray ray, const Scene& scene) override;

private:
    const bool kApplyRoughness = false;
    const double kAmbientFactor = 2e-3;
    static constexpr double kEpsilon = 1e-6;

    void CollectDirectLighting(Ray& ray, const Scene& scene, const Intersection& intersection);
};

}  // namespace Raytracer
