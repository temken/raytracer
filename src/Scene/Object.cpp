#include "Scene/Object.hpp"

namespace Raytracer {

Object::Object(const std::string& name, const Material& material, const Vector3D& position, const Vector3D& normal) :
    mName(name),
    mMaterial(material),
    mPosition(position),
    mNormal(normal.Normalized()) {
}

std::string Object::GetName() const {
    return mName;
}

void Object::SetVisible(bool visible) {
    mVisible = visible;
}

bool Object::IsVisible() const {
    return mVisible;
}

Material& Object::GetMaterial() {
    return mMaterial;
}

void Object::SetMaterial(const Material& material) {
    mMaterial = material;
}

Vector3D Object::GetPosition() const {
    return mPosition;
}

void Object::SetPosition(const Vector3D& position) {
    mPosition = position;
}

Vector3D Object::GetNormal() const {
    return mNormal;
}

void Object::SetNormal(const Vector3D& normal) {
    mNormal = normal;
}

Color Object::GetColor(const Intersection& intersection) const {
    return mMaterial.GetColor(intersection);
}

bool Object::EmitsLight() const {
    return mMaterial.EmitsLight();
}

Vector3D Object::GetVelocity() const {
    return mVelocity;
}

void Object::SetVelocity(const Vector3D& velocity) {
    mVelocity = velocity;
}

Vector3D Object::GetAngularVelocity() const {
    return mAngularVelocity;
}

void Object::SetAngularVelocity(const Vector3D& angularVelocity) {
    mAngularVelocity = angularVelocity;
}

Vector3D Object::GetSpin() const {
    return mSpin;
}

void Object::SetSpin(const Vector3D& spin) {
    mSpin = spin;
}

std::pair<double, double> Object::GetTextureCoordinates(const Vector3D& hitPoint) const {
    return {0.0, 0.0};
}

void Object::Evolve(double timeStep) {
    if (timeStep <= 0) {
        throw std::invalid_argument("Time step must be positive");
    }
    if (mVelocity.Norm() > sEpsilon) {
        Translate(mVelocity * timeStep);
    }
    if (mAngularVelocity.Norm() > sEpsilon) {
        Rotate(mAngularVelocity.Norm() * timeStep, mAngularVelocity.Normalized());
    }
    if (mSpin.Norm() > sEpsilon) {
        Spin(mSpin.Norm() * timeStep, mSpin.Normalized());
    }
}

void Object::PrintInfo() const {
    PrintInfoBase();
    mMaterial.PrintInfo();
}

void Object::Translate(const Vector3D& translation) {
    mPosition += translation;
}
void Object::Rotate(double angle, const Vector3D& axis) {
    // Implement rotation logic
}

void Object::Spin(double angle, const Vector3D& axis) {
    // Implement spin logic
}

void Object::PrintInfoBase() const {
    std::cout << "Object:" << mName << std::endl
              << "\tVisible:\t" << (mVisible ? "[x]" : "[ ]") << std::endl
              << "\tPosition:\t" << mPosition << std::endl
              << "\tNormal:\t" << mNormal << std::endl;

    if (mVelocity.Norm() > sEpsilon || mAngularVelocity.Norm() > sEpsilon || mSpin.Norm() > sEpsilon) {
        std::cout << "\tDynamic properties:" << std::endl
                  << "\tVelocity:\t" << mVelocity << std::endl
                  << "\tAngular Velocity:\t" << mAngularVelocity << std::endl
                  << "\tSpin:\t" << mSpin << std::endl;
    }
    std::cout << std::endl;
}

}  // namespace Raytracer
