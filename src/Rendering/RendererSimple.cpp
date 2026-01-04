#include "Rendering/RendererSimple.hpp"

namespace Raytracer {

Color RendererSimple::TraceRay(Ray ray, const Scene& scene) {
    auto intersection = Intersect(ray, scene);
    if (intersection) {
        return intersection->object->GetMaterial().GetColor(intersection.value());
    } else {
        return scene.GetBackgroundColor(ray);
    }
}

}  // namespace Raytracer
