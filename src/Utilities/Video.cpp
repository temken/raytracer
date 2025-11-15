#include "Utilities/Video.hpp"

#include "Utilities/Configuration.hpp"
#include "Version.hpp"

#include <filesystem>
#include <format>
#include <iostream>
#include <thread>

namespace Raytracer {

Video::Video(double fps) :
    mFPS(fps) {
}

Video::Video(const std::vector<Image>& images, double fps) :
    mFPS(fps) {
    for (const auto& img : images) {
        AddFrame(img);
    }
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
        std::filesystem::create_directories(outputDirectory + "/videos/");
        filepath = outputDirectory + "/videos/video_" + Configuration::GetInstance().GetRunID() + ".mp4";
    }

    std::filesystem::path outputFilepath = filepath;
    std::filesystem::path outputDirectory = outputFilepath.parent_path();

    // Create a temp subfolder for the frames
    std::filesystem::path tempFramesDir = outputDirectory / ("video_tmp_" + Configuration::GetInstance().GetRunID());
    std::filesystem::create_directories(tempFramesDir);

    for (std::size_t i = 0; i < mFrames.size(); i++) {
        const std::string frameFilename = std::format("frame_{:04}.png", static_cast<int>(i + 1));
        mFrames[i].Save(false, (tempFramesDir / frameFilename).string());
    }

    // ffmpeg command
    const std::string ffmpegExe = std::format("\"{}\"", FFMPEG_PATH);
    const std::string verbosity = showTerminalOutput ? "" : "-v warning";
    const std::string inputPattern = std::format("\"{}/frame_%04d.png\"", tempFramesDir.string());
    const std::string outputFile = std::format("\"{}\"", filepath);
    std::string ffmpegCommand = std::format(
        "{} -y {} -framerate {} -i {} -c:v libx264 -pix_fmt yuv420p {}",
        ffmpegExe, verbosity, mFPS, inputPattern, outputFile);

    std::system(ffmpegCommand.c_str());

    // Clean up temp folder if requested
    if (deleteFrameFiles) {
        std::error_code ec;
        std::filesystem::remove_all(tempFramesDir, ec);
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

void Video::PlayInTerminal(std::size_t width, bool loop, double terminalCharAspectRatio) const {
    if (mFrames.empty()) {
        throw std::runtime_error("No frames to play in terminal.");
    }

    double frameDuration = 1.0 / mFPS;

    double aspectRatio = static_cast<double>(mWidth) / static_cast<double>(mHeight);
    double height = static_cast<double>(width) / aspectRatio / terminalCharAspectRatio;
    std::size_t imageLines = static_cast<std::size_t>(height);

    std::cout << "\033[?25l";  // Hide cursor
    std::cout << std::endl;

    bool firstFrame = true;
    bool stop = false;
    while (!stop) {
        for (const auto& frame : mFrames) {
            if (!firstFrame) {
                // Move cursor up by the number of lines the image occupies and to the start of the line
                std::cout << "\033[" << imageLines + 1 << "A";
            }
            firstFrame = false;
            frame.PrintToTerminal(width, terminalCharAspectRatio);

            std::this_thread::sleep_for(
                std::chrono::duration<double>(frameDuration));
        }
        stop = !loop;
    }
    std::cout << "\033[?25h" << std::endl;  // Show cursor again
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
