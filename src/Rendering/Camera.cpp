#include "Rendering/Camera.hpp"

#include "Rendering/RendererDeterministic.hpp"
#include "Rendering/RendererPathTracer.hpp"
#include "Rendering/RendererPathTracerNEE.hpp"
#include "Rendering/RendererRayTracer.hpp"
#include "Rendering/RendererSimple.hpp"
#include "Utilities/Configuration.hpp"
#include "Utilities/Denoiser.hpp"

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

bool Camera::IsDynamic() const {
    return (mVelocity.Norm() > kEpsilon || mAngularVelocity.Norm() > kEpsilon || mSpin.Norm() > kEpsilon);
}

void Camera::Evolve(double timeStep) {
    Translate(mVelocity * timeStep);
    if (mAngularVelocity.Norm() > kEpsilon) {
        Rotate(mAngularVelocity.Norm() * timeStep, mAngularVelocity.Normalized());
    }
    if (mSpin.Norm() > kEpsilon) {
        Spin(mSpin.Norm() * timeStep, mSpin.Normalized());
    }
}

void Camera::InitializeOrbitTrajectory(double angularVelocity) {
    mEz = -1.0 * mPosition;
    mEz[2] = 0;
    mEz.Normalize();
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

void Camera::SetResolution(std::size_t width, std::size_t height) {
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

void Camera::SetSamplesPerPixel(std::size_t samples) {
    mSamplesPerPixel = samples;
}

void Camera::SetUseAntiAliasing(bool useAA) {
    mUseAntiAliasing = useAA;
}

void Camera::SetDenoisingMethod(Denoiser::Method method, std::size_t iterations) {
    mDenoisingMethod = method;
    mDenoisingIterations = iterations;
}

void Camera::SetRemoveHotPixels(bool remove) {
    mRemoveHotPixels = remove;
}

Image Camera::RenderImage(const Scene& scene, bool printProgressBar, bool createConvergingVideo) const {
    // Set the starting time
    auto startTime = std::chrono::high_resolution_clock::now();
    std::size_t samples = mSamplesPerPixel;
    if (mRenderer->IsDeterministic() && !mUseAntiAliasing && mSamplesPerPixel > 1) {
        std::cerr << "Warning: Renderer is deterministic, and anti-aliasing is disabled. Setting samples to 1." << std::endl;
        samples = 1;
    }

    // Some denoising methods need the G-Buffer
    const bool needGbuffer = !mRenderer->IsDeterministic() && mDenoisingMethod == Denoiser::Method::JOINT_BILATERAL_FILTER;
    std::optional<GBuffer> gBuffer;
    if (needGbuffer) {
        gBuffer.emplace(mResolution.width, mResolution.height);
    }

    std::size_t renderedPixels = 0;
    auto totalPixels = mResolution.width * mResolution.height * samples;
    std::vector<std::vector<Color>> accumulatedColors(mResolution.height, std::vector<Color>(mResolution.width, Color(0.0, 0.0, 0.0)));
    std::unique_ptr<Video> video = nullptr;
    if (createConvergingVideo && samples > 1) {
        video = std::make_unique<Video>(mFramesPerSecond);
    }
    for (std::size_t s = 0; s < samples; s++) {
#pragma omp parallel for collapse(2) schedule(static)
        for (std::size_t y = 0; y < mResolution.height; y++) {
            for (std::size_t x = 0; x < mResolution.width; x++) {
                if (s == 0 && gBuffer.has_value()) {
                    // Fill G-Buffer
                    const bool useAntiAliasingForGBuffer = false;
                    Ray gBufferRay = CreateRay(x, y, useAntiAliasingForGBuffer);
                    GBufferData gBufferData = mRenderer->ComputeGBuffer(gBufferRay, scene);
                    gBuffer->SetData(x, y, gBufferData);
                }

                // Sample the pixel
                Ray ray = CreateRay(x, y);
                Color pixel = mRenderer->TraceRay(ray, scene);

                accumulatedColors[y][x] += pixel;
            }
            if (printProgressBar) {
                std::size_t done;
#pragma omp atomic capture
                done = ++renderedPixels;

                if ((done % 200000) == 0 || done == totalPixels) {
                    double duration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
                    libphysica::Print_Progress_Bar(double(done) / double(totalPixels), 0, 60, duration, "Blue");
                }
            }
        }
        if (video) {
            Image tempImage = CreateRawImage(accumulatedColors, s + 1);
            video->AddFrame(tempImage);
        }
    }

    Image image = CreateRawImage(accumulatedColors, samples);
    ProcessImage(image, gBuffer);

    if (video) {
        video->AddFrame(image);
        std::string filepath = Configuration::GetInstance().GetOutputDirectory() + "/videos/converging_video_" + Configuration::GetInstance().GetRunID() + ".mp4";
        std::cout << "Saving converging video to: " << filepath << std::endl;
        video->Save(true, false, false, filepath);
    }

    double totalDuration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
    if (printProgressBar) {
        libphysica::Print_Progress_Bar(1.0, 0, 60, totalDuration, "Blue");
        std::cout << "\nRendered image with " << 1.0 / totalDuration << " FPS" << std::endl;
    }

    return image;
}

Video Camera::RenderVideo(Scene& scene, double durationSeconds, bool printProgressBar) {
    auto startTime = std::chrono::high_resolution_clock::now();

    std::size_t totalFrames = mFramesPerSecond * durationSeconds;
    double timeStep = 1.0 / mFramesPerSecond;
    Video video(mFramesPerSecond);
    for (std::size_t i = 0; i < totalFrames; i++) {
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

// Better function name? Create Rendered Image? ConstructImage?
Image Camera::CreateRawImage(const std::vector<std::vector<Color>>& accumulatedColors, std::size_t samples) const {
    Image image(mResolution.width, mResolution.height);
    for (std::size_t y = 0; y < mResolution.height; y++) {
        for (std::size_t x = 0; x < mResolution.width; x++) {
            Color accumulatedColor = accumulatedColors[y][x];
            Color colorAverage((accumulatedColor.R() / samples), (accumulatedColor.G() / samples), (accumulatedColor.B() / samples));
            image.SetPixel(x, y, colorAverage);
        }
    }
    return image;
}

void Camera::ProcessImage(Image& image, std::optional<GBuffer>& gBuffer) const {
    // 1. Remove outliers in linear space
    if (mRemoveHotPixels) {
        image = Denoiser::RemoveHotPixels(image);
    }
    // 2. Denoise in linear space for Monte Carlo renderers
    if (!mRenderer->IsDeterministic()) {
        Denoiser::ApplyDenoising(image, mDenoisingMethod, gBuffer, mDenoisingIterations);
    }
    // 3. Exposure adjustment
    image.ApplyGammaCorrection();
    // 4. Tone mapping
    image.ApplyReinhardToneMapping();
    // 5. Display transform
    image.ConvertLinearToSRGB();
}

void Camera::PrintInfo() const {
    std::cout << "Camera Information:" << std::endl
              << "Renderer:\t" << mRenderer->GetTypeString() << std::endl
              << "Position:\t" << mPosition << std::endl
              << "Direction:\t" << mEz << std::endl
              << "Field of View:\t" << mFieldOfView << std::endl
              << "Resolution:\t" << mResolution.width << "x" << mResolution.height << std::endl
              << "Post-Processing:" << std::endl
              << "\tRemove Hot Pixels:\t" << (mRemoveHotPixels ? "[x]" : "[ ]") << std::endl
              << "\tDenoising Method:\t" << Denoiser::MethodToString(mDenoisingMethod) << " (Iterations: " << mDenoisingIterations << ")" << std::endl
              << "FPS:\t\t" << mFramesPerSecond << std::endl
              << "Samples/Pixel:\t" << mSamplesPerPixel << std::endl
              << "Anti-Aliasing:\t" << (mUseAntiAliasing ? "[x]" : "[ ]") << std::endl
              << "Dynamic:\t" << (IsDynamic() ? "[x]" : "[ ]") << std::endl;
    if (IsDynamic()) {
        std::cout << "Velocity:\t" << mVelocity << std::endl
                  << "Angular Vel.:\t" << mAngularVelocity << std::endl
                  << "Spin:\t\t" << mSpin << std::endl;
    }
    std::cout << std::endl;
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

Ray Camera::CreateRay(std::size_t x, std::size_t y, bool useAntiAliasing) const {
    const double width = double(mResolution.width);
    const double height = double(mResolution.height);

    // Left-positive, up-positive pixel-center offsets on the image plane
    double dx = 0.0;
    double dy = 0.0;
    if (mUseAntiAliasing && useAntiAliasing) {
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
        case Renderer::Type::RAY_TRACER:
            return std::make_unique<RendererRayTracer>();
        case Renderer::Type::PATH_TRACER:
            return std::make_unique<RendererPathTracer>();
        case Renderer::Type::DETERMINISTIC:
            return std::make_unique<RendererDeterministic>();
        case Renderer::Type::PATH_TRACER_NEE:
            return std::make_unique<RendererPathTracerNEE>();
        default:
            throw std::invalid_argument("Unknown renderer type");
    }
}

}  // namespace Raytracer
