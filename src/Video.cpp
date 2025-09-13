#include "Rendering/Video.hpp"

#include "Utilities/Configuration.hpp"
#include "version.hpp"

#include <filesystem>
#include <format>
#include <iostream>

namespace Raytracer {

Video::Video(double fps) :
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

void Video::Save(bool openFile, bool showTerminalOutput, bool deleteFrameFiles, std::string filepath) {
    if (mFrames.empty()) {
        std::cerr << "No frames to save." << std::endl;
        return;
    }

    if (filepath.empty()) {
        std::string outputDirectory = Configuration::GetInstance().GetOutputDirectory();
        // Create /videos/ folder if it does not exist
        std::filesystem::create_directories(outputDirectory + "/videos/");
        filepath = outputDirectory + "/videos/video_" + std::to_string(std::time(nullptr)) + ".mp4";
    }
    std::filesystem::path outputFilepath = filepath;
    std::filesystem::path outputDirectory = outputFilepath.parent_path();

    // Save video frames to individual files
    for (std::size_t i = 0; i < mFrames.size(); i++) {
        const std::string frameFilename = std::format("frame_{:04}.png", static_cast<int>(i + 1));

        mFrames[i].Save(false, (outputDirectory / frameFilename).string());
    }

    // Generate video from frame files
    std::string ffmpegCommand = std::format(
        "{} -y {} -framerate {} -i {}/frame_%04d.png -c:v libx264 -pix_fmt yuv420p {}",
        FFMPEG_PATH,
        showTerminalOutput ? "" : "-v warning",
        mFPS,
        outputDirectory.string(),
        filepath);
    std::system(ffmpegCommand.c_str());

    if (deleteFrameFiles) {
        for (std::size_t i = 0; i < mFrames.size(); i++) {
            std::filesystem::remove((outputDirectory / std::format("frame_{:04}.png", static_cast<int>(i + 1))).string());
        }
    }

    if (openFile) {
        std::string command;
#ifdef __APPLE__
        command = "open " + filepath;
#elif __linux__
        command = "xdg-open " + filepath;
#elif _WIN32
        command = "start " + filepath;
#endif
        std::system(command.c_str());
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
