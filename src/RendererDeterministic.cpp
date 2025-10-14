#include "Rendering/RendererDeterministic.hpp"

namespace Raytracer {

Color RendererDeterministic::TraceRay(Ray ray, const Scene& scene) {
    auto intersection = Intersect(ray, scene);
    if (intersection) {
        Material::InteractionType mostLikelyInteraction = intersection->material->MostLikelyInteraction();
        switch (mostLikelyInteraction) {
            case Material::InteractionType::DIFFUSE:
                return intersection->material->GetColor();
            case Material::InteractionType::REFLECTIVE:
                intersection->material->Reflect(ray, intersection->point, intersection->normal, kApplyRoughness);
                break;
            case Material::InteractionType::REFRACTIVE:
                intersection->material->Refract(ray, intersection->point, intersection->normal, kApplyRoughness);
                break;
        }
    } else {
        return ray.GetColor() * scene.GetBackgroundColor();
    }
    return TraceRay(ray, scene);
}

}  // namespace Raytracer
