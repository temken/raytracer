#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>  // for strlen
#include <iostream>
#include <memory>

#include "Rendering/Camera.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/Ray.hpp"
#include "Scene/Box.hpp"
#include "Scene/Disk.hpp"
#include "Scene/Rectangle.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Sphere.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/Configuration.hpp"
#include "Utilities/Vector.hpp"
#include "version.hpp"

using namespace Raytracer;

int main(int argc, char** argv) {
    //Initial terminal output
    auto time_start = std::chrono::system_clock::now();
    auto time_start_t = std::chrono::system_clock::to_time_t(time_start);
    auto* ctime_start = ctime(&time_start_t);
    if (ctime_start[std::strlen(ctime_start) - 1] == '\n') {
        ctime_start[std::strlen(ctime_start) - 1] = '\0';
    }
    std::cout << "[Started on " << ctime_start << "]" << std::endl;
    std::cout << PROJECT_NAME << "-" << PROJECT_VERSION << "\tgit:" << GIT_BRANCH << "/" << GIT_COMMIT_HASH << std::endl
              << std::endl;
    ////////////////////////////////////////////////////////////////////////
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config.yaml>\n";
        return 1;
    } else {
        std::cout << "Using configuration file: " << argv[1] << std::endl;
    }
    try {
        Configuration::GetInstance().ParseYamlFile(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing configuration file: " << e.what() << std::endl;
        return 1;
    }
    ////////////////////////////////////////////////////////////////////////

    Configuration::GetInstance().PrintInfo();
    Camera camera = Configuration::GetInstance().ConstructCamera();
    Scene scene = Configuration::GetInstance().ConstructScene();

    camera.PrintInfo();
    scene.PrintInfo();

    bool openOutputFiles = true;
    Image image = camera.Render(scene, true);
    image.Save(openOutputFiles);

    std::cout << "\n\nBlack pixel ratio: " << image.CalculateBlackPixelRatio() * 100.0 << "%" << std::endl;

    // Video video = camera.RenderOrbitVideo(scene, 60);
    // video.Save(openOutputFiles);

    ////////////////////////////////////////////////////////////////////////
    //Final terminal output
    auto time_end = std::chrono::system_clock::now();
    double durationTotal = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start).count();
    std::cout << "\n[Finished in " << std::round(1000. * durationTotal) / 1000. << "s";
    if (durationTotal > 60.0) {
        std::cout << " (" << floor(durationTotal / 3600.0) << ":" << floor(fmod(durationTotal / 60.0, 60.0)) << ":" << floor(fmod(durationTotal, 60.0)) << ")]." << std::endl;
    } else {
        std::cout << "]" << std::endl;
    }

    return 0;
}
