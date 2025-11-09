#include "Geometry/Shapes/Disk.hpp"

namespace Raytracer::Geometry {

Disk::Disk(const Vector3D& position, const Vector3D& normal, double radius) :
    Ring(position, normal, 0.0, radius) {
    mType = Type::DISK;
}

std::vector<Vector3D> Disk::GetKeyPoints() const {
    return {mPosition};
}

void Disk::PrintInfo() const {
    PrintInfoBase();
    std::cout << "\tRadius:\t" << mOuterRadius << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
