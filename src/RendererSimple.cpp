#include "Rendering/RendererSimple.hpp"

namespace Raytracer {

Color RendererSimple::TraceRay(Ray ray, const Scene& scene) {
    auto intersection = Intersect(ray, scene);
    if (intersection) {
        if (intersection->object && intersection->object->IsReflective()) {
            Vector3D reflectDir = ray.GetDirection() - 2 * ray.GetDirection().Dot(intersection->normal) * intersection->normal;
            Ray reflectRay(intersection->point, reflectDir);
            return TraceRay(reflectRay, scene);
        } else {
            return intersection->object->GetColor();
        }
    }
    return scene.GetBackgroundColor();
}

}  // namespace Raytracer
