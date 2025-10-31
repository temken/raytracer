#include "Rendering/Renderer.hpp"

namespace Raytracer {

Renderer::Renderer(Type type, bool deterministic) :
    mType(type),
    mIsDeterministic(deterministic) {}

bool Renderer::IsDeterministic() const {
    return mIsDeterministic;
}

Renderer::Type Renderer::GetType() const {
    return mType;
}

std::string Renderer::GetTypeString() const {
    switch (mType) {
        case Type::SIMPLE:
            return "Simple";
        case Type::DETERMINISTIC:
            return "Deterministic";
        case Type::RAY_TRACER:
            return "Ray Tracer";
        case Type::PATH_TRACER:
            return "Path Tracer";
    }
}

std::optional<Intersection> Renderer::Intersect(const Ray& ray, const Scene& scene) {
    std::optional<Intersection> closestIntersection;
    for (const auto& object : scene.GetObjects()) {
        if (!object || !object->IsVisible()) {
            continue;
        }

        if (auto intersection = object->Intersect(ray)) {
            if (intersection->t > kEpsilon && (!closestIntersection || intersection->t < closestIntersection->t)) {
                closestIntersection.emplace(*intersection);
            }
        }
    }
    return closestIntersection;
}

void Renderer::CollectDirectLighting(Ray& ray, const Scene& scene, const Intersection& intersection, std::size_t numLightSamples) {
    const auto& material = intersection.object->GetMaterial();
    const Vector3D& x = intersection.point;
    Vector3D n = intersection.normal.Normalized();

    if (ray.IsEntering(n)) {
        n = -1.0 * n;
    }

    bool anyLightHit = false;
    Color directRadiance(0.0, 0.0, 0.0);
    for (const auto& lightSource : scene.GetLightSources()) {
        const double lightArea = lightSource->GetSurfaceArea();

        const Color Le = lightSource->GetMaterial().GetEmission();  // emitted radiance (RGB)

        const std::vector<Vector3D> lightPoints = mIsDeterministic ? lightSource->GetKeyPoints() : lightSource->SampleSurfacePoints(numLightSamples, mGenerator);

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
            anyLightHit = true;
            const Vector3D nL = shadowHit->normal.Normalized();

            const double cosSurface = std::max(0.0, n.Dot(toLight));
            const double cosLight = std::max(0.0, nL.Dot((-1.0) * toLight));

            if (cosSurface <= 0.0 || cosLight <= 0.0) {
                continue;
            }

            // Lambertian BRDF
            const Color f_r = material.GetColor(intersection) * (1.0 / M_PI);

            // Geometry factor
            const double G = cosSurface * cosLight * lightArea / dist2;

            // Direct contribution
            colorSum += f_r * Le * G;
            lightHits++;
        }

        if (lightHits > 0) {
            directRadiance += colorSum / static_cast<double>(lightHits);
        }
    }

    if (!anyLightHit) {
        ray.AddRadiance(kAmbientFactor * ray.GetThroughput());
    } else {
        ray.AddRadiance(directRadiance);
        // It could be that this is actually physically more correct, but it looks less nice.
        // ray.AddRadiance(ray.GetThroughput() * directRadiance);
    }
}

}  // namespace Raytracer
