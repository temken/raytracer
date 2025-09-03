#include "Camera.hpp"

namespace Raytracer {

Camera::Camera() :
    mPosition({0, 0, 0}),
    mEz({1, 0, 0}) {
    CreateOrthogonalBasis();
}

Camera::Camera(const Vector3D& position, const Vector3D& direction) :
    mPosition(position),
    mEz(direction) {
    CreateOrthogonalBasis();
}

void Camera::SetPosition(const Vector3D& position) {
    mPosition = position;
}

void Camera::SetDirection(const Vector3D& direction) {
    mEz = direction.Normalized();
    CreateOrthogonalBasis();
}

void Camera::SetFieldOfView(float fov) {
    mFieldOfView = fov;
}

void Camera::SetResolution(size_t width, size_t height) {
    // Throw error for odd values
    if (width % 2 != 0 || height % 2 != 0) {
        throw std::invalid_argument("Resolution must be even numbers.");
    }
    mResolution = {width, height};
}

Image Camera::Render(const Scene& scene) const {
    Image image(mResolution[0], mResolution[1]);
    for (size_t y = 0; y < mResolution[1]; y++) {
        for (size_t x = 0; x < mResolution[0]; x++) {
            Ray ray = CreateRay(x, y);
            Color color = Renderer::TraceRay(ray, scene);
            image.SetPixel(x, y, color);
        }
    }
    return image;
}

Ray Camera::CreateRay(size_t x, size_t y) const {
    return Ray();
}

void Camera::CreateOrthogonalBasis() {
    // The eX vector points horizontally to the left of eZ
    mEx = mEz.Cross(Vector3D({0, 0, 1})).Normalized();
    // The eY vector points vertically up
    mEy = mEz.Cross(mEx).Normalized();
}

}  // namespace Raytracer
