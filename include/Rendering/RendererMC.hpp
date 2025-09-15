#pragma once

#include "Rendering/Renderer.hpp"

#include <random>

namespace Raytracer {

class RendererMC : public Renderer {
public:
    RendererMC();

    virtual Color TraceRay(Ray ray, const Scene& scene) override;

private:
    size_t mMaximumInteractions = 10;
    std::mt19937 mGenerator{std::random_device{}()};
    std::uniform_real_distribution<double> mDistribution{0.0, 1.0};
};

}  // namespace Raytracer
