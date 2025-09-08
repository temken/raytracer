#pragma once

#include "Image.hpp"

#include <string>

namespace Raytracer {

class Video {
public:
    Video(const std::string& name, double fps = 30.0);

    double GetDuration() const;  // in seconds

    void SetFrameRate(double fps);
    double GetFrameRate() const;

    void AddFrame(const Image& image);
    void Save(bool showTerminalOutput = false, bool deleteFrameFiles = true);

    void PrintInfo() const;

private:
    std::string name;
    size_t mWidth = 0;
    size_t mHeight = 0;
    double mFPS = 30.0;
    std::vector<Image> mFrames;
};

}  // namespace Raytracer
