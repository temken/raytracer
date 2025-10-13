#include "Rendering/RendererSimple.hpp"

namespace Raytracer {

Color RendererSimple::TraceRay(Ray ray, const Scene& scene) {
    auto intersection = Intersect(ray, scene);
    if (intersection) {
        return intersection->material->GetColor();
    } else {
        return scene.GetBackgroundColor();
    }
}

}  // namespace Raytracer
