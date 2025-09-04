#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>  // for strlen
#include <iostream>
#include <memory>

#include "Camera.hpp"
#include "Color.hpp"
#include "Image.hpp"
#include "Ray.hpp"
#include "Scene.hpp"
#include "Sphere.hpp"
#include "Vector.hpp"
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
    scene.AddObject(std::make_unique<Sphere>(Vector3D({0, 0, 0.25}), 1, Color(1, 0, 0)));   // Red sphere
    scene.AddObject(std::make_unique<Sphere>(Vector3D({2, 0, -0.25}), 1, Color(0, 1, 0)));  // Green sphere
    scene.AddObject(std::make_unique<Sphere>(Vector3D({0, 2, 0.5}), 1, Color(0, 0, 1)));    // Blue sphere
    scene.AddObject(std::make_unique<Sphere>(Vector3D({0, -2, 0}), 1, Color(1, 1, 0)));     // Yellow sphere

    scene.PrintSceneInfo();

    // Create Camera
    Camera camera;
    camera.SetFieldOfView(90.0);
    camera.SetResolution(800, 600);

    camera.SetPosition(Vector3D({10, 0, 0}));
    camera.SetDirection(Vector3D({-1, 0, 0}));

    camera.PointToOrigin(3, 6.0, 3.5 * M_PI / 2.0);

    camera.PrintInfo();

    Image image = camera.Render(scene);
    image.Save("output.png");
    std::system("open images/output.png");

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
