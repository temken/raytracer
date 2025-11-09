#include "Geometry/Shape.hpp"

namespace Raytracer::Geometry {

Shape::Shape(Type type, const Vector3D& position, const Vector3D& orientation, const Vector3D& referenceDirection) :
    mType(type),
    mPosition(position),
    mOrthonormalBasis(orientation, referenceDirection) {
}

Vector3D Shape::GetBasisVector(OrthonormalBasis::BasisVector axis) const {
    return mOrthonormalBasis.GetBasisVector(axis);
}

Vector3D Shape::GetOrientation() const {
    return GetBasisVector(OrthonormalBasis::BasisVector::eZ);
}

Vector3D Shape::GetPosition() const {
    return mPosition;
}

void Shape::SetPosition(const Vector3D& newPosition) {
    mPosition = newPosition;
}

std::pair<double, double> Shape::GetSurfaceParameters(const Vector3D& point) const {
    return {0.0, 0.0};
}

void Shape::Rotate(double angle, const Line& axis) {
    Vector3D axisDirection = axis.GetDirection().Normalized();

    // Vector from a point on the axis to the object's center
    Vector3D r = GetPosition() - axis.GetOrigin();

    // Rodrigues' rotation formula:
    Vector3D r_rot =
        r * std::cos(angle) +
        axisDirection.Cross(r) * std::sin(angle) +
        axisDirection * (axisDirection.Dot(r)) * (1.0 - std::cos(angle));

    SetPosition(axis.GetOrigin() + r_rot);
}

void Shape::Spin(double angle, Vector3D axis) {
    mOrthonormalBasis.Rotate(angle, axis);
}

void Shape::PrintInfo() const {
    PrintInfoBase();
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
        case Type::HALF_SPHERE:
            return "Half Sphere";
        case Type::PLANE:
            return "Plane";
        case Type::RECTANGLE:
            return "Rectangle";
        case Type::RING:
            return "Ring";
        case Type::SPHERE:
            return "Sphere";
        case Type::TETRAHEDRON:
            return "Tetrahedron";
        case Type::TRIANGLE:
            return "Triangle";
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
