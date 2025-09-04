#include "Video.hpp"

#include "version.hpp"

#include <filesystem>
#include <format>
#include <iostream>

namespace Raytracer {

Video::Video(const std::string& name, double fps) :
    name(name),
    mFPS(fps) {
}

double Video::GetDuration() const {
    return mFrames.size() / mFPS;
}

void Video::SetFrameRate(double fps) {
    mFPS = fps;
}
double Video::GetFrameRate() const {
    return mFPS;
}

void Video::AddFrame(const Image& image) {
    if (mFrames.empty()) {
        mWidth = image.GetWidth();
        mHeight = image.GetHeight();
    } else if (image.GetWidth() != mWidth || image.GetHeight() != mHeight) {
        throw std::invalid_argument("Frame dimensions do not match video dimensions.");
    }
    mFrames.push_back(image);
}

void Video::Save(bool deleteFrameFiles) {
    if (mFrames.empty()) {
        std::cerr << "No frames to save." << std::endl;
        return;
    }

    // Create a folder in /output/
    std::filesystem::path dir = PROJECT_DIR "videos/" + name + "/";
    std::filesystem::create_directories(dir);

    // Save video frames to individual files
    for (std::size_t i = 0; i < mFrames.size(); i++) {
        const std::string frameFilename = std::format("frame_{:04}.png", static_cast<int>(i + 1));

        mFrames[i].Save((dir / frameFilename).string());
    }

    // Generate video from frame files
    std::string videoFilename = dir / (name + ".mp4");
    std::string ffmpegCommand = std::format(
        "{} -framerate {} -i {}/frame_%04d.png -c:v libx264 -pix_fmt yuv420p {}",
        FFMPEG_PATH,
        mFPS,
        dir.string(),
        videoFilename);
    std::system(ffmpegCommand.c_str());

    if (deleteFrameFiles) {
        for (std::size_t i = 0; i < mFrames.size(); i++) {
            std::filesystem::remove((dir / std::format("frame_{:04}.png", static_cast<int>(i + 1))).string());
        }
    }
}

void Video::PrintInfo() const {
    std::cout << "Video Info:" << std::endl;
    std::cout << "Resolution: " << mWidth << "x" << mHeight << std::endl;
    std::cout << "Frame Rate: " << mFPS << " FPS" << std::endl;
    std::cout << "Duration: " << GetDuration() << " seconds" << std::endl;
    std::cout << "Number of Frames: " << mFrames.size() << std::endl
              << std::endl;
}

}  // namespace Raytracer
