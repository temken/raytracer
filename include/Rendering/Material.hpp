#pragma once

#include "Rendering/Ray.hpp"
#include "Utilities/Color.hpp"

#include <map>
#include <random>

namespace Raytracer {

class Material {
public:
    enum class InteractionType {
        DIFFUSE,
        REFLECTIVE,
        REFRACTIVE
    };

    Material();
    Material(const Color& color, double roughness = 1.0, double refractiveIndex = 1.0, double meanFreePath = 0.0, double luminance = 0.0);

    void Interact(Ray& ray, const Vector3D& intersectionPoint, const Vector3D& normal, bool applyRoughness = true);

    void Diffuse(Ray& incomingRay, const Vector3D& intersectionPoint, const Vector3D& normal);
    void Reflect(Ray& incomingRay, const Vector3D& intersectionPoint, const Vector3D& normal, bool applyRoughness);
    void Refract(Ray& incomingRay, const Vector3D& intersectionPoint, const Vector3D& normal, bool applyRoughness);

    Color GetColor() const;
    void SetColor(const Color& color);

    Color GetColorSpecular() const;
    void SetColorSpecular(const Color& color);

    double GetRoughness() const;
    void SetRoughness(double roughness);

    double GetRefractiveIndex() const;
    void SetRefractiveIndex(double refractiveIndex);

    double GetMeanFreePath() const;
    void SetMeanFreePath(double meanFreePath);

    bool EmitsLight() const;
    double GetRadiance(double distance = 0.0) const;
    void SetRadiance(double radiance);

    void SetInteractionProbabilities(const std::map<InteractionType, double>& probs);
    InteractionType MostLikelyInteraction() const;

    void PrintInfo() const;

private:
    Color mColor;
    Color mColorSpecular;
    double mRoughness;        // 0 = perfect mirror, 1 = very rough
    double mRefractiveIndex;  // 1 = vacuum, 1.33 = water, 1.5 = glass
    double mMeanFreePath;     // Average distance a photon travels in the material before interacting
    double mRadiance;         // Brightness of the material if it emits light

    // Probability for each interaction type
    std::map<InteractionType, double> probabilities;

    // Random numbers:
    std::mt19937 mGenerator{std::random_device{}()};
    std::uniform_real_distribution<double> mDistribution{0.0, 1.0};

    static constexpr double kEpsilon = 1e-5;

    void NormalizeProbabilities();

    Vector3D SampleCone(const Vector3D& axis, double angle);
};

}  // namespace Raytracer
