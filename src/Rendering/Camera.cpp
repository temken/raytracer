#include "Rendering/Camera.hpp"

#include "Rendering/RendererDeterministic.hpp"
#include "Rendering/RendererMC.hpp"
#include "Rendering/RendererSimple.hpp"
#include "Utilities/Configuration.hpp"

#include "libphysica/Utilities.hpp"

#include <omp.h>
#include <chrono>
#include <random>

namespace Raytracer {

Camera::Camera(Renderer::Type rendererType) :
    mPosition({0, 0, 0}),
    mEz({1, 0, 0}),
    mRenderer(CreateRenderer(rendererType)) {
    ConfigureCamera();
}

Camera::Camera(const Vector3D& position, const Vector3D& direction, Renderer::Type rendererType) :
    mPosition(position),
    mEz(direction),
    mRenderer(CreateRenderer(rendererType)) {
    ConfigureCamera();
}

void Camera::SetPosition(const Vector3D& position) {
    mPosition = position;
}

void Camera::SetDirection(const Vector3D& direction) {
    mEz = direction.Normalized();
    ConfigureCamera();
}

// Dynamics
void Camera::SetVelocity(const Vector3D& velocity) {
    mVelocity = velocity;
}

void Camera::SetAngularVelocity(const Vector3D& angularVelocity) {
    mAngularVelocity = angularVelocity;
}

void Camera::SetSpin(const Vector3D& spin) {
    mSpin = spin;
}

void Camera::InitializeOrbitTrajectory(double angularVelocity) {
    mEz = -1.0 * mPosition.Normalized();
    ConfigureCamera();

    // Set angular and spin velocity for orbit around z axis
    Vector3D angularVelocityVec({0, 0, angularVelocity});
    Vector3D spinVec({0, 0, angularVelocity});
    SetAngularVelocity(angularVelocityVec);
    SetVelocity(Vector3D());
    SetSpin(spinVec);
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
    ConfigureCamera();
}

void Camera::SetFramesPerSecond(double fps) {
    mFramesPerSecond = fps;
}

void Camera::SetSamplesPerPixel(size_t samples) {
    mSamplesPerPixel = samples;
}

void Camera::SetUseAntiAliasing(bool useAA) {
    mUseAntiAliasing = useAA;
}

void Camera::SetBlurImage(bool blur) {
    mBlurImage = blur;
}

Image Camera::RenderImage(const Scene& scene, bool printProgressBar, bool createConvergingVideo) const {
    // Set the starting time
    auto startTime = std::chrono::high_resolution_clock::now();
    size_t samples = mSamplesPerPixel;
    if (mRenderer->IsDeterministic() && !mUseAntiAliasing && mSamplesPerPixel > 1) {
        std::cerr << "Warning: Renderer is deterministic, and anti-aliasing is disabled. Setting samples to 1." << std::endl;
        samples = 1;
    }

    size_t renderedPixels = 0;
    auto totalPixels = mResolution.width * mResolution.height * samples;
    std::vector<std::vector<Color>> accumulatedColors(mResolution.height, std::vector<Color>(mResolution.width, Color(0.0, 0.0, 0.0)));

    std::unique_ptr<Video> video = nullptr;
    if (createConvergingVideo && samples > 1) {
        video = std::make_unique<Video>(mFramesPerSecond);
    }
    for (size_t s = 0; s < samples; s++) {
#pragma omp parallel for collapse(2) schedule(static)
        for (size_t y = 0; y < mResolution.height; y++) {
            for (size_t x = 0; x < mResolution.width; x++) {
                // Sample the pixel
                Ray ray = CreateRay(x, y);
                Color color = mRenderer->TraceRay(ray, scene);
                accumulatedColors[y][x] += color;
            }
            if (printProgressBar) {
                size_t done;
#pragma omp atomic capture
                done = ++renderedPixels;

                if ((done % 200000) == 0 || done == totalPixels) {
                    double duration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
                    libphysica::Print_Progress_Bar(double(done) / double(totalPixels), 0, 60, duration, "Blue");
                }
            }
        }
        if (video) {
            Image tempImage = CreateImage(accumulatedColors, s + 1, true);
            video->AddFrame(tempImage);
        }
    }
    if (video) {
        std::string filepath = Configuration::GetInstance().GetOutputDirectory() + "/videos/converging_video_" + std::to_string(std::time(nullptr)) + ".mp4";
        std::cout << "Saving converging video to: " << filepath << std::endl;
        video->Save(true, false, false, filepath);
    }
    bool applyPostProcessing = (mRenderer->GetType() == Renderer::Type::MONTE_CARLO && samples > 1);
    Image image = CreateImage(accumulatedColors, samples, applyPostProcessing);
    double totalDuration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
    if (printProgressBar) {
        libphysica::Print_Progress_Bar(1.0, 0, 60, totalDuration, "Blue");
        std::cout << "\nRendered image with " << 1.0 / totalDuration << " FPS" << std::endl;
    }

    return image;
}

Video Camera::RenderVideo(Scene& scene, double durationSeconds, bool printProgressBar) {
    auto startTime = std::chrono::high_resolution_clock::now();

    size_t totalFrames = mFramesPerSecond * durationSeconds;
    double timeStep = 1.0 / mFramesPerSecond;
    Video video(mFramesPerSecond);
    for (size_t i = 0; i < totalFrames; i++) {
        Image frame = RenderImage(scene);
        video.AddFrame(frame);
        if (printProgressBar) {
            double duration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
            libphysica::Print_Progress_Bar(double(i + 1) / totalFrames, 0, 60, duration, "Red");
        }
        Evolve(timeStep);
        scene.Evolve(timeStep);
    }
    if (printProgressBar) {
        double totalDuration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
        std::cout << "\nRendered video with " << totalFrames / totalDuration << " FPS" << std::endl;
    }
    return video;
}

Image Camera::CreateImage(const std::vector<std::vector<Color>>& accumulatedColors, size_t samples, bool applyPostProcessing) const {
    Image image(mResolution.width, mResolution.height);
    for (size_t y = 0; y < mResolution.height; y++) {
        for (size_t x = 0; x < mResolution.width; x++) {
            Color accumulatedColor = accumulatedColors[y][x];
            Color colorAverage((accumulatedColor.R() / samples), (accumulatedColor.G() / samples), (accumulatedColor.B() / samples));

            // if (mRenderer->GetType() == Renderer::Type::MONTE_CARLO && samples > 1) {
            if (applyPostProcessing && samples > 1) {
                ApplyPostProcessing(colorAverage);
            }

            image.SetPixel(x, y, colorAverage);
        }
    }

    if (mBlurImage) {
        image.Blur();
    }
    return image;
}

void Camera::ApplyPostProcessing(Color& color) {
    ApplyGammaCorrection(color);
    ReinhardToneMapping(color);
    LinearToSRGB(color);
}

void Camera::ApplyGammaCorrection(Color& color) {
    // Gamma correction for MC renderer
    // Apply exposure (EV = 1.5) and gain (2^(EV))
    double exposureEV = 1.5;
    double gain = std::pow(2.0, exposureEV);
    color = color * gain;
}

void Camera::ReinhardToneMapping(Color& color) {
    double R = color.R() / (1.0 + color.R());
    double G = color.G() / (1.0 + color.G());
    double B = color.B() / (1.0 + color.B());
    color = Color(R, G, B);
}

void Camera::LinearToSRGB(Color& color) {
    auto to_srgb = [](double x) {
        x = std::min(1.0, std::max(0.0, x));
        return (x <= 0.0031308) ? 12.92 * x : 1.055 * std::pow(x, 1.0 / 2.4) - 0.055;
    };
    color = Color(to_srgb(color.R()), to_srgb(color.G()), to_srgb(color.B()));
}

void Camera::PrintInfo() const {
    std::cout << "Camera Information:" << std::endl
              << "Renderer:\t" << mRenderer->GetTypeString() << std::endl
              << "Position:\t" << mPosition << std::endl
              << "Direction:\t" << mEz << std::endl
              << "Field of View:\t" << mFieldOfView << std::endl
              << "Resolution:\t" << mResolution.width << "x" << mResolution.height << std::endl
              << "FPS:\t\t" << mFramesPerSecond << std::endl
              << "Samples/Pixel:\t" << mSamplesPerPixel << std::endl
              << "Anti-Aliasing:\t" << (mUseAntiAliasing ? "[x]" : "[ ]") << std::endl
              << "Blur Image:\t" << (mBlurImage ? "[x]" : "[ ]") << std::endl
              << std::endl;
}

void Camera::Translate(const Vector3D& translation) {
    mPosition += translation;
}

void Camera::Rotate(double angle, const Vector3D& axis) {
    // Rotate around axis through the origin
    // Camera orientation remains unchanged

    // TODO: Put rotate into a utility function or Vector class
    Vector3D axisNorm = axis.Normalized();
    Vector3D rotatedPosition = (mPosition * std::cos(angle)) +
                               (axisNorm.Cross(mPosition) * std::sin(angle)) +
                               (axisNorm * (axisNorm.Dot(mPosition)) * (1.0 - std::cos(angle)));
    mPosition = rotatedPosition;
}

void Camera::Spin(double angle, const Vector3D& axis) {
    // Rotate around axis through the camera position
    Vector3D pos = mPosition;
    Translate(-1.0 * pos);

    // TODO: Put rotate into a utility function or Vector class
    Vector3D axisNorm = axis.Normalized();
    Vector3D rotatedEz = (mEz * std::cos(angle)) +
                         (axisNorm.Cross(mEz) * std::sin(angle)) +
                         (axisNorm * (axisNorm.Dot(mEz)) * (1.0 - std::cos(angle)));
    Translate(pos);
    mEz = rotatedEz.Normalized();
    ConfigureCamera();
}

void Camera::Evolve(double timeStep) {
    Translate(mVelocity * timeStep);
    Rotate(mAngularVelocity.Norm() * timeStep, mAngularVelocity.Normalized());
    Spin(mSpin.Norm() * timeStep, mSpin.Normalized());
    ConfigureCamera();
}

Ray Camera::CreateRay(size_t x, size_t y) const {
    const double width = double(mResolution.width);
    const double height = double(mResolution.height);

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
    mEx = -1.0 * mEz.Cross(Vector3D({0, 0, 1})).Normalized();
    // The eY vector points vertically up
    mEy = mEz.Cross(mEx).Normalized();

    // 2. Compute pixel size
    mPixelSize = 2.0 * mDistance * std::tan(mFieldOfView * 0.5 * M_PI / 180.0) / mResolution.width;
}

std::unique_ptr<Renderer> Camera::CreateRenderer(Renderer::Type type) {
    switch (type) {
        case Renderer::Type::SIMPLE:
            return std::make_unique<RendererSimple>();
        case Renderer::Type::MONTE_CARLO:
            return std::make_unique<RendererMC>();
        case Renderer::Type::DETERMINISTIC:
            return std::make_unique<RendererDeterministic>();
        default:
            throw std::invalid_argument("Unknown renderer type");
    }
}

}  // namespace Raytracer
