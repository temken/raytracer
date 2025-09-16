#include "Rendering/RendererMC.hpp"

namespace Raytracer {

RendererMC::RendererMC() :
    Renderer(Type::MONTE_CARLO, false) {
}

Color RendererMC::TraceRay(Ray ray, const Scene& scene) {
    Color throughput(1.0, 1.0, 1.0);
    size_t interactions = 0;
    while (interactions < mMaximumInteractions) {
        auto intersection = Intersect(ray, scene);
        if (!intersection.has_value()) {
            return throughput * scene.GetBackgroundColor();
        }
        const auto* object = intersection->object;

        if (object->EmitsLight()) {
            return throughput * object->GetColor();
        }

        Vector3D newDir;

        if (object->IsReflective()) {
            newDir = ray.GetDirection() - 2 * ray.GetDirection().Dot(intersection->normal) * intersection->normal;
            throughput = throughput * object->GetColor();
        } else {
            // Diffuse surface: random new direction in hemisphere
            // Build ONB around normal
            Vector3D eZ = intersection->normal;
            Vector3D a = (std::fabs(eZ[0]) > 0.707) ? Vector3D({0.0, 1.0, 0.0}) : Vector3D({1.0, 0.0, 0.0});
            Vector3D eX = a.Cross(eZ).Normalized();
            Vector3D eY = eZ.Cross(eX);

            // Cosine-weighted hemisphere sample in local coords
            double u1 = mDistribution(mGenerator);
            double u2 = mDistribution(mGenerator);
            double cosTheta = std::sqrt(u1);
            double sinTheta = std::sqrt(1.0 - u1);
            double phi = 2.0 * M_PI * u2;
            double x = sinTheta * std::cos(phi);
            double y = sinTheta * std::sin(phi);

            // Transform to world and normalize
            newDir = (x * eX + y * eY + cosTheta * eZ).Normalized();

            // Lambertian: throughput *= albedo
            throughput = throughput * object->GetColor();
        }

        ray = Ray(intersection->point, newDir);

        // Russian roulette after a few bounces
        if (interactions >= 3) {
            double p = std::max({throughput.R(), throughput.G(), throughput.B()});
            p = std::clamp(p, 0.05, 0.95);
            if (mDistribution(mGenerator) > p) {
                return BLACK;
            }
            throughput = throughput * (1.0 / p);
        }

        interactions++;
    }
    return throughput * scene.GetBackgroundColor();
}

}  // namespace Raytracer
