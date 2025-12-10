#include "Scene/Object.hpp"

namespace Raytracer {

Object::Object(Type type, const std::string& name) : mType(type), mName(name) {}

std::string Object::GetName() const {
    return mName;
}

bool Object::IsDynamic() const {
    return (mVelocity.Norm() > 0.0 || mAcceleration.Norm() > 0.0 ||
            mAngularVelocity.Norm() > 0.0 || mSpin.Norm() > 0.0);
}

void Object::SetVelocity(const Vector3D& velocity) {
    mVelocity = velocity;
}

void Object::SetAcceleration(const Vector3D& acceleration) {
    mAcceleration = acceleration;
}

void Object::SetAngularVelocity(const Vector3D& angularVelocity) {
    mAngularVelocity = angularVelocity;
}

void Object::SetSpin(const Vector3D& spin) {
    mSpin = spin;
}

void Object::PrintInfoBase() const {
    std::cout << "Object Summary:" << std::endl
              << "\tName:\t" << mName << std::endl
              << "\tType:\t" << (mType == Type::PRIMITIVE ? "Primitive" : "Composite") << std::endl
              << "\tVisible:\t" << (IsVisible() ? "[X]" : "[ ]") << std::endl;
    if (IsDynamic()) {
        std::cout << "\n\tDynamic Properties:" << std::endl
                  << "\t\tVelocity:\t" << mVelocity << std::endl
                  << "\t\tAcceleration:\t" << mAcceleration << std::endl
                  << "\t\tAngular Velocity:\t" << mAngularVelocity << std::endl
                  << "\t\tSpin:\t" << mSpin << std::endl;
    }
}

}  // namespace Raytracer
