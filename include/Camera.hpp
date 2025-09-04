#pragma once

#include "Image.hpp"
#include "Ray.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Vector.hpp"
#include "Video.hpp"

namespace Raytracer {

class Camera {
public:
    Camera();
    Camera(const Vector3D& position, const Vector3D& direction);

    void SetPosition(const Vector3D& position);
    void SetDirection(const Vector3D& direction);
    void SetFieldOfView(double fov);
    void SetResolution(size_t width, size_t height);

    Image Render(const Scene& scene) const;
    Video FlyAround(const Scene& scene, double distance, double height, size_t numFrames, double fps = 30.0);

    void PointToOrigin(double height, double rho, double phi);

    void PrintInfo() const;

private:
    Vector3D mPosition;
    // Basis vectors, where the camera points in z direction, and x points horizontally to the left, y points vertically up
    Vector3D mEx = Vector3D({1, 0, 0});
    Vector3D mEy = Vector3D({0, 1, 0});
    Vector3D mEz = Vector3D({0, 0, 1});

    double mFieldOfView = 90.0;  // in degrees
    double mDistance = 1.0;      // Distance from the camera to the image plane
    double mPixelSize;           // at distance 1 from camera
    std::array<size_t, 2> mResolution = {800, 600};

    std::unique_ptr<Renderer> mRenderer;

    Ray CreateRay(size_t x, size_t y) const;

    void ConfigureCamera();
};

}  // namespace Raytracer
