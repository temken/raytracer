#include "Utilities/Denoiser.hpp"

#include <algorithm>
#include <cmath>

namespace Raytracer {

Image Denoiser::Denoise(const Image& inputImage, Denoiser::Method method) {
    switch (method) {
        case Method::BLUR:
            return Blur(inputImage, 1);
        case Method::GAUSSIAN_BLUR:
            return GaussianBlur(inputImage, 1.0);
        default:
            throw std::invalid_argument("Unsupported denoising method.");
    }
}

Image Denoiser::Blur(const Image& inputImage, std::size_t blurCount) {
    if (inputImage.GetWidth() < 3 || inputImage.GetHeight() < 3) {
        return inputImage;  // Image too small to blur
    }

    Image outputImage = inputImage;
    // Simple box blur implementation
    for (std::size_t i = 0; i < blurCount; i++) {
        for (std::size_t y = 1; y < outputImage.GetHeight() - 1; y++) {
            for (std::size_t x = 1; x < outputImage.GetWidth() - 1; x++) {
                Color sum(0.0, 0.0, 0.0);
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        sum += outputImage.GetPixel(x + dx, y + dy);
                    }
                }
                outputImage.SetPixel(x, y, sum * (1.0 / 9.0));
            }
        }
    }
    return outputImage;
}

double Denoiser::Gaussian(double x, double sigma) {
    return (1.0 / (sigma * std::sqrt(2.0 * M_PI))) * std::exp(-(x * x) / (2.0 * sigma * sigma));
}

std::vector<std::vector<double>> Denoiser::CreateGaussianKernel(double sigma) {
    std::size_t kernelRadius = static_cast<std::size_t>(std::ceil(3 * sigma));
    std::vector<std::vector<double>> kernel(2 * kernelRadius + 1, std::vector<double>(2 * kernelRadius + 1));
    double sum = 0.0;
    for (int y = -static_cast<int>(kernelRadius); y <= static_cast<int>(kernelRadius); y++) {
        for (int x = -static_cast<int>(kernelRadius); x <= static_cast<int>(kernelRadius); x++) {
            double r = std::sqrt(x * x + y * y);
            double value = Gaussian(r, sigma);
            kernel[y + static_cast<int>(kernelRadius)][x + static_cast<int>(kernelRadius)] = value;
            sum += value;
        }
    }

    // Normalize kernel
    for (int y = 0; y < 2 * kernelRadius + 1; ++y) {
        for (int x = 0; x < 2 * kernelRadius + 1; ++x) {
            kernel[y][x] /= sum;
        }
    }
    return kernel;
}

Image Denoiser::GaussianBlur(const Image& inputImage, double sigma) {
    int kernelRadius = static_cast<int>(std::ceil(3 * sigma));
    if (kernelRadius < 1) {
        return inputImage;
    }
    // Create Gaussian kernel
    auto kernel = CreateGaussianKernel(sigma);

    Image outputImage = inputImage;
    for (std::size_t y = 0; y < inputImage.GetHeight(); ++y) {
        for (std::size_t x = 0; x < inputImage.GetWidth(); ++x) {
            Color sum(0.0, 0.0, 0.0);
            double weightSum = 0.0;
            for (int ky = -kernelRadius; ky <= kernelRadius; ++ky) {
                for (int kx = -kernelRadius; kx <= kernelRadius; ++kx) {
                    if (x + kx < 0 || x + kx >= inputImage.GetWidth() ||
                        y + ky < 0 || y + ky >= inputImage.GetHeight()) {
                        continue;
                    }
                    Color pixel = inputImage.GetPixel(x + kx, y + ky);
                    double weight = kernel[ky + kernelRadius][kx + kernelRadius];
                    sum += pixel * weight;
                    weightSum += weight;
                }
            }
            if (weightSum > 0.0) {
                outputImage.SetPixel(x, y, sum / weightSum);
            }
        }
    }

    return outputImage;
}

Image Denoiser::RemoveHotPixels(const Image& inputImage) {
    Image outputImage = inputImage;

    for (std::size_t y = 1; y < inputImage.GetHeight() - 1; ++y) {
        for (std::size_t x = 1; x < inputImage.GetWidth() - 1; ++x) {
            Color currentPixel = inputImage.GetPixel(x, y);
            std::vector<Color> neighbors = inputImage.GetNeighbors(x, y);

            // Simple average of neighbors
            Color averageColor(0.0, 0.0, 0.0);
            for (const auto& neighbor : neighbors) {
                averageColor += neighbor;
            }
            averageColor = averageColor / static_cast<double>(neighbors.size());

            // If current pixel deviates significantly from average, replace it
            const double threshold = 0.5;
            if (std::abs(currentPixel.R() - averageColor.R()) > threshold ||
                std::abs(currentPixel.G() - averageColor.G()) > threshold ||
                std::abs(currentPixel.B() - averageColor.B()) > threshold) {
                outputImage.SetPixel(x, y, averageColor);
            }
        }
    }
    return outputImage;
}

}  // namespace Raytracer
