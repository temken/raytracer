#include "Rendering/RendererDeterministic.hpp"

namespace Raytracer {

Color RendererDeterministic::TraceRay(Ray ray, const Scene& scene) {
    auto intersection = Intersect(ray, scene);
    if (intersection) {
        Material::InteractionType mostLikelyInteraction = intersection->object->GetMaterial().MostLikelyInteraction();
        switch (mostLikelyInteraction) {
            case Material::InteractionType::DIFFUSE:
                return intersection->object->GetMaterial().GetColor(intersection.value());
            case Material::InteractionType::REFLECTIVE:
                intersection->object->GetMaterial().Reflect(ray, intersection.value(), kApplyRoughness);
                break;
            case Material::InteractionType::REFRACTIVE:
                intersection->object->GetMaterial().Refract(ray, intersection.value(), kApplyRoughness);
                break;
        }
    } else {
        return ray.GetColor() * scene.GetBackgroundColor();
    }
    return TraceRay(ray, scene);
}

}  // namespace Raytracer
