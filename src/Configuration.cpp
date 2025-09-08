#include "Utilities/Configuration.hpp"

#include "version.hpp"

#include <filesystem>
#include <iostream>

namespace Raytracer {

Configuration& Configuration::GetInstance() {
    static Configuration instance;
    return instance;
}

void Configuration::ParseYamlFile(const std::string& path) {
}

std::string Configuration::GetID() const {
    return mID;
}

Camera Configuration::GetCamera() const {
    return mCamera;
}

Scene Configuration::GetScene() const {
    return mScene;
}

std::string Configuration::GetImagesDirectory() const {
    return PROJECT_DIR "output/" + mID + "/images/";
}

std::string Configuration::GetVideosDirectory() const {
    return PROJECT_DIR "output/" + mID + "/videos/";
}

void Configuration::PrintInfo() const {
    std::cout << "Configuration Information:" << std::endl;
    std::cout << "ID: " << mID << std::endl;
    mCamera.PrintInfo();
    mScene.PrintInfo();
}

void Configuration::CreateOutputDirectory() const {
    std::filesystem::create_directories(GetImagesDirectory());
    std::filesystem::create_directories(GetVideosDirectory());
}

}  // namespace Raytracer
