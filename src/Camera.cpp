#include "Rendering/Camera.hpp"

#include "libphysica/Utilities.hpp"

#include <random>

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
    ConfigureCamera();
}

void Camera::SetResolution(size_t width, size_t height) {
    // Throw error for odd values
    if (width % 2 != 0 || height % 2 != 0) {
        throw std::invalid_argument("Resolution must be even numbers.");
    }
    mResolution = {width, height};
}

void Camera::SetUseAntiAliasing(bool useAA) {
    mUseAntiAliasing = useAA;
}

Image Camera::Render(const Scene& scene, size_t samples, bool printProgressBar) const {
    Image image(mResolution[0], mResolution[1]);

    if (mRenderer.IsDeterministic() && !mUseAntiAliasing && samples > 1) {
        std::cerr << "Warning: Renderer is deterministic, and anti-aliasing is disabled. Setting samples to 1." << std::endl;
        samples = 1;
    }

    size_t pixelsRendered = 0;
    auto totalPixels = mResolution[0] * mResolution[1];
    for (size_t y = 0; y < mResolution[1]; y++) {
        for (size_t x = 0; x < mResolution[0]; x++) {
            double redAccumulated = 0.0;
            double greenAccumulated = 0.0;
            double blueAccumulated = 0.0;
            double alphaAccumulated = 0.0;
            for (size_t s = 0; s < samples; s++) {
                // Sample the pixel
                Ray ray = CreateRay(x, y);
                Color color = mRenderer.TraceRay(ray, scene);
                redAccumulated += color.R();
                greenAccumulated += color.G();
                blueAccumulated += color.B();
                alphaAccumulated += color.A();
            }
            Color finalColor((redAccumulated / samples), (greenAccumulated / samples), (blueAccumulated / samples), (alphaAccumulated / samples));
            image.SetPixel(x, y, finalColor);
            if (printProgressBar && pixelsRendered++ % 1000 == 0) {
                libphysica::Print_Progress_Bar(double(pixelsRendered) / totalPixels);
            }
        }
    }
    return image;
}

Video Camera::FlyAround(const Scene& scene, double distance, double height, size_t numFrames, size_t samplesPerFrame, double fps) {
    Video video("fly_around", fps);
    for (size_t i = 0; i < numFrames; i++) {
        double phi = 2.0 * M_PI * i / numFrames;
        PointToOrigin(height, distance, phi);
        Image frame = Render(scene, samplesPerFrame);
        video.AddFrame(frame);
        libphysica::Print_Progress_Bar(double(i + 1) / numFrames);
    }
    return video;
}

void Camera::PointToOrigin(double height, double rho, double phi) {
    Vector3D position({rho * std::cos(phi), rho * std::sin(phi), height});
    SetPosition(position);
    SetDirection(-1.0 * position);
}

void Camera::PrintInfo() const {
    std::cout << "Camera Information:" << std::endl;
    std::cout << "Position:\t" << mPosition << std::endl;
    std::cout << "Direction:\t" << mEz << std::endl;
    std::cout << "Field of View:\t" << mFieldOfView << std::endl;
    std::cout << "Resolution:\t" << mResolution[0] << "x" << mResolution[1] << std::endl;
    std::cout << "Anti-Aliasing:\t" << (mUseAntiAliasing ? "[x]" : "[ ]") << std::endl
              << std::endl;
}

Ray Camera::CreateRay(size_t x, size_t y) const {
    const double width = double(mResolution[0]);
    const double height = double(mResolution[1]);

    // Left-positive, up-positive pixel-center offsets on the image plane
    double dx = 0.0;
    double dy = 0.0;
    if (mUseAntiAliasing) {
        static thread_local std::mt19937 generator(std::random_device{}());
        static thread_local std::uniform_real_distribution<double> distribution(-0.5, 0.5);
        dx = distribution(generator);
        dy = distribution(generator);
    }
    const double u = (0.5 * width - (double(x) + 0.5) + dx) * mPixelSize;
    const double v = (0.5 * height - (double(y) + 0.5) + dy) * mPixelSize;

    Vector3D direction = (mEz * mDistance) + (mEx * u) + (mEy * v);
    return Ray(mPosition, direction.Normalized());
}

void Camera::ConfigureCamera() {
    // 1. Create orthogonal basis
    // The eX vector points horizontally to the left of eZ
    mEx = mEz.Cross(Vector3D({0, 0, 1})).Normalized();
    // The eY vector points vertically up
    mEy = mEz.Cross(mEx).Normalized();

    // 2. Compute pixel size
    mPixelSize = 2.0 * mDistance * std::tan(mFieldOfView * 0.5 * M_PI / 180.0) / mResolution[0];
}

}  // namespace Raytracer
