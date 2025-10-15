#pragma once

#include "Image.hpp"

#include <string>

namespace Raytracer {

class Video {
public:
    Video(double fps = 30.0);
    Video(const std::vector<Image>& images, double fps = 30.0);

    double GetDuration() const;  // in seconds

    void SetFrameRate(double fps);
    double GetFrameRate() const;

    void AddFrame(const Image& image);
    void Save(bool openFile = false, bool showTerminalOutput = false, bool deleteFrameFiles = true, std::string filepath = "");

    void PrintInfo() const;

private:
    size_t mWidth = 0;
    size_t mHeight = 0;
    double mFPS = 30.0;
    std::vector<Image> mFrames;
};

}  // namespace Raytracer
