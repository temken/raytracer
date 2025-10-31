#include "Geometry/Shape.hpp"

namespace Raytracer::Geometry {

Shape::Shape(Type type, const Vector3D& position, const Vector3D& orientation, const Vector3D& referenceDirection) :
    mType(type),
    mPosition(position),
    mBasis(orientation, referenceDirection) {
}

const OrthonormalBasis& Shape::GetBasis() const {
    return mBasis;
}

Vector3D Shape::GetOrientation() const {
    return mBasis.GetBasisVector(OrthonormalBasis::BasisVector::eZ);
}

std::pair<double, double> Shape::SurfaceParameters(const Vector3D& point) const {
    return {0.0, 0.0};
}

void Shape::PrintInfo() const {
    PrintInfoBase();
}

void Shape::Rotate(double angle, const Vector3D& axis) {
    mBasis.Rotate(angle, axis);
}

void Shape::Spin(double angle, OrthonormalBasis::BasisVector basisVector) {
    mBasis.Rotate(angle, basisVector);
}

std::string Shape::TypeToString(Type type) {
    switch (type) {
        case Type::BOX:
            return "Box";
        case Type::BOX_AXIS_ALIGNED:
            return "Box (Axis Aligned)";
        case Type::CYLINDER:
            return "Cylinder";
        case Type::DISK:
            return "Disk";
        case Type::PLANE:
            return "Plane";
        case Type::RECTANGLE:
            return "Rectangle";
        case Type::SPHERE:
            return "Sphere";
        case Type::TUBE:
            return "Tube";
    }
}

void Shape::PrintInfoBase() const {
    std::cout << "Shape: " << TypeToString(mType) << std::endl
              << "\tPosition:\t" << mPosition << std::endl
              << "\tOrientation:\t" << GetOrientation() << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
