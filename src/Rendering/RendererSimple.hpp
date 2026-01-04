#pragma once

#include "Rendering/Ray.hpp"
#include "Rendering/Renderer.hpp"
#include "Scene/Scene.hpp"
#include "Utilities/Color.hpp"

namespace Raytracer {

class RendererSimple : public Renderer {
public:
    RendererSimple() :
        Renderer(Type::SIMPLE, true) {};

    virtual Color TraceRay(Ray ray, const Scene& scene) override;

private:
};

}  // namespace Raytracer
