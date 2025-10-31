#pragma once

#include "Geometry/Vector.hpp"
#include "Rendering/Ray.hpp"
#include "Rendering/Renderer.hpp"
#include "Scene/Scene.hpp"
#include "Utilities/Image.hpp"
#include "Utilities/Video.hpp"

namespace Raytracer {

class Camera {
public:
    Camera(Renderer::Type rendererType);
    Camera(const Vector3D& position, const Vector3D& direction, Renderer::Type rendererType);

    struct Resolution {
        size_t width{800};
        size_t height{600};
    };

    void SetPosition(const Vector3D& position);
    void SetDirection(const Vector3D& direction);

    // Dynamics
    void SetVelocity(const Vector3D& velocity);
    void SetAngularVelocity(const Vector3D& angularVelocity);
    void SetSpin(const Vector3D& spin);

    void InitializeOrbitTrajectory(double angularVelocity);

    // Settings
    void SetFieldOfView(double fov);
    void SetResolution(size_t width, size_t height);
    void SetFramesPerSecond(double fps);
    void SetSamplesPerPixel(size_t samples);
    void SetUseAntiAliasing(bool useAA);
    void SetBlurImage(bool blur);

    Image RenderImage(const Scene& scene, bool printProgressBar = false, bool createConvergingVideo = false) const;
    Video RenderVideo(Scene& scene, double durationSeconds, bool printProgressBar = true);

    void PrintInfo() const;

private:
    Vector3D mPosition;
    Vector3D mVelocity = Vector3D();
    Vector3D mAngularVelocity = Vector3D();
    Vector3D mSpin = Vector3D();

    // Basis vectors, where the camera points in z direction, and x points horizontally to the left, y points vertically up
    Vector3D mEx = Vector3D({1, 0, 0});
    Vector3D mEy = Vector3D({0, 1, 0});
    Vector3D mEz = Vector3D({0, 0, 1});

    double mFieldOfView = 90.0;  // in degrees
    double mDistance = 1.0;      // Distance from the camera to the image plane
    double mPixelSize;           // at distance 1 from camera
    Camera::Resolution mResolution;
    double mFramesPerSecond = 30.0;

    std::unique_ptr<Renderer> mRenderer;
    size_t mSamplesPerPixel = 1;
    bool mUseAntiAliasing = false;
    bool mBlurImage = false;

    // Camera dynamics
    void Translate(const Vector3D& translation);
    void Rotate(double angle, const Vector3D& axis = Vector3D({0, 0, 1}));
    void Spin(double angle, const Vector3D& axis = Vector3D({0, 0, 1}));
    void Evolve(double timeStep);

    Ray CreateRay(size_t x, size_t y) const;

    Image CreateImage(const std::vector<std::vector<Color>>& accumulatedColors, size_t samples, bool applyPostProcessing) const;

    static void ApplyPostProcessing(Color& color);
    static void ApplyGammaCorrection(Color& color);
    static void ReinhardToneMapping(Color& color);
    static void LinearToSRGB(Color& color);

    void ConfigureCamera();
    static std::unique_ptr<Renderer> CreateRenderer(Renderer::Type type);
};

}  // namespace Raytracer
