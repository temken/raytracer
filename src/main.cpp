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
#include "Utilities/Vector.hpp"
#include "version.hpp"

using namespace Raytracer;

int main() {
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

    //Create Scene
    Scene scene;

    // Add objects to the scene

    // Spheres
    // Sphere reflectiveSphere(Vector3D({0, 3, 0.5}), 1, WHITE);
    // reflectiveSphere.SetReflective(true);
    // scene.AddObject(std::make_unique<Sphere>(reflectiveSphere));
    // scene.AddObject(std::make_unique<Sphere>(Vector3D({0, 0, 0.25}), 1, RED));
    // scene.AddObject(std::make_unique<Sphere>(Vector3D({4, 0, -0.25}), 1, CYAN));
    // // scene.AddObject(std::make_unique<Sphere>(Vector3D({0, 3, 0.5}), 1, BLUE));
    // scene.AddObject(std::make_unique<Sphere>(Vector3D({0, -3, 0}), 1, YELLOW));
    // scene.AddObject(std::make_unique<Sphere>(Vector3D({-3, 0, 1}), 1, GREEN));

    // // Disk
    // Disk mirror(Vector3D({0, -9.9, 0}), Vector3D({0, 1, 0}), 3, WHITE);
    // mirror.SetReflective(true);
    // Disk mirror2(Vector3D({0, 9.9, 0}), Vector3D({0, -1, 0}), 3, WHITE);
    // mirror2.SetReflective(true);
    // scene.AddObject(std::make_unique<Disk>(mirror));
    // scene.AddObject(std::make_unique<Disk>(mirror2));
    // scene.AddObject(std::make_unique<Disk>(Vector3D({0, 0, 4.9}), Vector3D({0, 0, -1}), 1.5, YELLOW));

    // // Box
    // scene.AddObject(std::make_unique<Box>(Vector3D({3, -4, -4}), 2, 2, 2, std::array<Color, 6>{RED, GREEN, BLUE, CYAN, YELLOW, GREEN}));

    // // Stage box
    // scene.AddObject(std::make_unique<Rectangle>(Vector3D({0, 0, -5}), Vector3D({0, 0, 1}), 10, 20, GRAY));
    // scene.AddObject(std::make_unique<Rectangle>(Vector3D({0, 0, +5}), Vector3D({0, 0, 1}), 10, 20, GRAY));
    // scene.AddObject(std::make_unique<Rectangle>(Vector3D({0, +10, 0}), Vector3D({0, 1, 0}), 10, 10, BLUE));
    // scene.AddObject(std::make_unique<Rectangle>(Vector3D({0, -10, 0}), Vector3D({0, 1, 0}), 10, 10, RED));
    // // Background
    // scene.AddObject(std::make_unique<Rectangle>(Vector3D({-5, -8.75, 0}), Vector3D({1, 0, 0}), 10, 2.5, BLACK));
    // scene.AddObject(std::make_unique<Rectangle>(Vector3D({-5, -6.25, 0}), Vector3D({1, 0, 0}), 10, 2.5, WHITE));
    // scene.AddObject(std::make_unique<Rectangle>(Vector3D({-5, -3.75, 0}), Vector3D({1, 0, 0}), 10, 2.5, BLACK));
    // scene.AddObject(std::make_unique<Rectangle>(Vector3D({-5, -1.25, 0}), Vector3D({1, 0, 0}), 10, 2.5, WHITE));
    // scene.AddObject(std::make_unique<Rectangle>(Vector3D({-5, 1.25, 0}), Vector3D({1, 0, 0}), 10, 2.5, BLACK));
    // scene.AddObject(std::make_unique<Rectangle>(Vector3D({-5, 3.75, 0}), Vector3D({1, 0, 0}), 10, 2.5, WHITE));
    // scene.AddObject(std::make_unique<Rectangle>(Vector3D({-5, 6.25, 0}), Vector3D({1, 0, 0}), 10, 2.5, BLACK));
    // scene.AddObject(std::make_unique<Rectangle>(Vector3D({-5, 8.75, 0}), Vector3D({1, 0, 0}), 10, 2.5, WHITE));

    // scene.PrintSceneInfo();

    // Create Camera
    Camera camera;
    camera.SetFieldOfView(90.0);
    camera.SetResolution(1600, 1200);
    camera.SetUseAntiAliasing(false);

    camera.SetPosition(Vector3D({15, 0, 0}));
    camera.SetDirection(Vector3D({-1, 0, 0}));

    // camera.PointToOrigin(0, 15.0, 0);

    camera.PrintInfo();

    Image image = camera.Render(scene, 100, true);
    image.Save(PROJECT_DIR "images/output2.png");
    std::system("open " PROJECT_DIR "images/output2.png");

    Video video = camera.FlyAround(scene, 10.0, 0.0, 60);
    video.Save();
    std::system("open " PROJECT_DIR "videos/fly_around/fly_around.mp4");

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
