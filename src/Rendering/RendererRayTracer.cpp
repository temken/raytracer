#include "Rendering/RendererRayTracer.hpp"

namespace Raytracer {

RendererRayTracer::RendererRayTracer() :
    Renderer(Type::RAY_TRACER, false) {
}

Color RendererRayTracer::TraceRay(Ray ray, const Scene& scene) {
    while (ray.GetDepth() < kMaximumDepth) {
        auto intersection = Intersect(ray, scene);
        if (!intersection.has_value()) {
            return scene.GetBackgroundColor();
        }

        auto& material = intersection->object->GetMaterial();

        if (material.EmitsLight()) {
            ray.AddRadiance(ray.GetThroughput() * material.GetEmission());
            break;
        }

        auto interactionType = material.Interact(ray, intersection.value());

        if (interactionType == Material::InteractionType::DIFFUSE) {
            CollectDirectLighting(ray, scene, intersection.value());
            break;
        }
    }

    return ray.GetRadiance();
}

void RendererRayTracer::CollectDirectLighting(Ray& ray, const Scene& scene, const Intersection& intersection) {
    const auto& material = intersection.object->GetMaterial();
    const Vector3D& x = intersection.point;
    const Vector3D n = intersection.normal.Normalized();

    Color directRadiance(0.0, 0.0, 0.0);

    for (const auto& lightSource : scene.GetLightSources()) {
        const double lightArea = lightSource->GetSurfaceArea();

        const Color Le = lightSource->GetMaterial().GetEmission();  // emitted radiance (RGB)

        const std::vector<Vector3D> lightPoints = lightSource->SampleSurfacePoints(kNumLightSamples, mGenerator);

        Color colorSum(0.0, 0.0, 0.0);

        std::size_t lightHits = 0;
        for (const Vector3D& y : lightPoints) {
            Vector3D toLight = y - x;
            const double dist2 = toLight.NormSquared();
            toLight.Normalize();

            Ray shadowRay(x + toLight * kEpsilon, toLight);
            auto shadowHit = Intersect(shadowRay, scene);
            if (!shadowHit.has_value() || shadowHit->object != lightSource.get()) {
                continue;  // occluded or no intersection
            }

            const Vector3D nL = shadowHit->normal.Normalized();

            const double cosSurface = std::max(0.0, n.Dot(toLight));
            const double cosLight = std::max(0.0, nL.Dot((-1.0) * toLight));

            if (cosSurface <= 0.0 || cosLight <= 0.0) {
                continue;
            }

            // Lambertian BRDF
            const Color f_r = material.GetColor(intersection) * (1.0 / M_PI);

            // Geometry factor
            const double G = (cosSurface * cosLight) / dist2;

            // Direct contribution (include area weighting for Monte Carlo sampling)
            colorSum += f_r * Le * G;
            lightHits++;
        }

        // Monte Carlo integration: (Area/N) * Σ(contributions)
        directRadiance += (colorSum * lightArea) / static_cast<double>(lightHits);
    }

    // Add this direct lighting contribution (throughput already modified by material interaction)
    ray.AddRadiance(directRadiance);
}

}  // namespace Raytracer
