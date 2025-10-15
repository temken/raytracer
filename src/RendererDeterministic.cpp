#include "Rendering/RendererDeterministic.hpp"

namespace Raytracer {

Color RendererDeterministic::TraceRay(Ray ray, const Scene& scene) {
    while (ray.GetDepth() < kMaximumDepth) {
        auto intersection = Intersect(ray, scene);
        if (!intersection.has_value()) {
            return ray.GetColor() * scene.GetBackgroundColor();
        }
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
    }
    return ray.GetRadiance() * ray.GetColor();
}

}  // namespace Raytracer
