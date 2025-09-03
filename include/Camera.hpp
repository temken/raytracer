#pragma once

#include "Image.hpp"
#include "Ray.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Vector.hpp"

namespace Raytracer {

class Camera {
public:
    Camera();
    Camera(const Vector3D& position, const Vector3D& direction);

    void SetPosition(const Vector3D& position);
    void SetDirection(const Vector3D& direction);
    void SetFieldOfView(float fov);
    void SetResolution(size_t width, size_t height);

    Image Render(const Scene& scene) const;

private:
    Vector3D mPosition;
    // Basis vectors, where the camera points in z direction, and x points horizontally to the left, y points vertically up
    Vector3D mEx = Vector3D({1, 0, 0});
    Vector3D mEy = Vector3D({0, 1, 0});
    Vector3D mEz = Vector3D({0, 0, 1});

    float mFieldOfView = 45.0f;
    std::array<size_t, 2> mResolution = {800, 600};

    std::unique_ptr<Renderer> mRenderer;

    Ray CreateRay(size_t x, size_t y) const;

    void CreateOrthogonalBasis();
};

}  // namespace Raytracer
