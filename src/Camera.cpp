#include "Camera.hpp"

namespace Raytracer {

Camera::Camera() :
    mPosition({0, 0, 0}),
    mEz({1, 0, 0}) {
    ConfigureCamera();
}

Camera::Camera(const Vector3D& position, const Vector3D& direction) :
    mPosition(position),
    mEz(direction) {
    ConfigureCamera();
}

void Camera::SetPosition(const Vector3D& position) {
    mPosition = position;
}

void Camera::SetDirection(const Vector3D& direction) {
    mEz = direction.Normalized();
    ConfigureCamera();
}

void Camera::SetFieldOfView(double fov) {
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
            // std::cout << "Rendering pixel (" << x << ", " << y << "): " << color << std::endl;
        }
    }
    return image;
}

void Camera::PrintInfo() const {
    std::cout << "Camera Information:" << std::endl;
    std::cout << "Position: " << mPosition << std::endl;
    std::cout << "Direction: " << mEz << std::endl;
    std::cout << "Field of View: " << mFieldOfView << std::endl;
    std::cout << "Resolution: " << mResolution[0] << "x" << mResolution[1] << std::endl
              << std::endl;
}

Ray Camera::CreateRay(size_t x, size_t y) const {
    const double width = double(mResolution[0]);
    const double height = double(mResolution[1]);

    // Left-positive, up-positive pixel-center offsets on the image plane
    const double u = (0.5 * width - (double(x) + 0.5)) * mPixelSize;
    const double v = (0.5 * height - (double(y) + 0.5)) * mPixelSize;

    Vector3D direction = (mEz * mDistance) + (mEx * u) + (mEy * v);
    // std::cout << "Creating ray for pixel (" << x << ", " << y << "): " << direction << std::endl;
    return Ray(mPosition, direction.Normalized());
}

void Camera::ConfigureCamera() {
    // 1. Create orthogonal basis
    // The eX vector points horizontally to the left of eZ
    mEx = mEz.Cross(Vector3D({0, 0, 1})).Normalized();
    // The eY vector points vertically up
    mEy = mEz.Cross(mEx).Normalized();

    // 2. Compute pixel size
    mPixelSize = 2.0 * mDistance * std::tan(mFieldOfView * 0.5 * M_PI / 180.0) / mResolution[1];
}

}  // namespace Raytracer
