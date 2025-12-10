#pragma once

#include "Rendering/Ray.hpp"
#include "Rendering/Renderer.hpp"
#include "Scene/Scene.hpp"
#include "Utilities/Color.hpp"

namespace Raytracer {

class RendererDeterministic : public Renderer {
public:
    RendererDeterministic();

    virtual Color TraceRay(Ray ray, const Scene& scene) override;

private:
};

}  // namespace Raytracer
