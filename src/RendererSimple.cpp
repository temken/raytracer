#include "Rendering/RendererSimple.hpp"

namespace Raytracer {

Color RendererSimple::TraceRay(Ray ray, const Scene& scene) {
    auto intersection = Intersect(ray, scene);
    if (intersection) {
        return intersection->object->GetMaterial().GetBaseColor();
    } else {
        return scene.GetBackgroundColor();
    }
}

}  // namespace Raytracer
