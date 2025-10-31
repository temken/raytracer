#pragma once

#include "Rendering/Renderer.hpp"

namespace Raytracer {

class RendererRayTracer : public Renderer {
public:
    RendererRayTracer();

    virtual Color TraceRay(Ray ray, const Scene& scene) override;

private:
    static constexpr size_t kNumLightSamples = 5;
};

}  // namespace Raytracer
