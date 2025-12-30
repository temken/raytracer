#include "Utilities/Denoiser.hpp"

#include <algorithm>

namespace Raytracer {

Image Denoiser::Denoise(const Image& inputImage, Denoiser::Method method) {
    switch (method) {
        case Method::BLUR:
            return Blur(inputImage, 1);
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
