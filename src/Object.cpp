#include "Scene/Object.hpp"

namespace Raytracer {

Object::Object(const std::string& name, const Color& color, const Vector3D& position, const Vector3D& normal) :
    mName(name),
    mPosition(position),
    mNormal(normal.Normalized()),
    mColor(color) {}

std::string Object::GetName() const {
    return mName;
}

void Object::SetVisible(bool visible) {
    mVisible = visible;
}

bool Object::IsVisible() const {
    return mVisible;
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

Color Object::GetColor() const {
    return mColor;
}

void Object::SetColor(const Color& color) {
    mColor = color;
}

bool Object::EmitsLight() const {
    return mEmitsLight;
}

void Object::SetEmitsLight(bool emitsLight) {
    mEmitsLight = emitsLight;
}

bool Object::IsReflective() const {
    return mIsReflective;
}

void Object::SetReflective(bool isReflective) {
    mIsReflective = isReflective;
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

void Object::Translate(const Vector3D& translation) {
    mPosition += translation;
}
void Object::Rotate(double angle, const Vector3D& axis) {
    // Implement rotation logic
}

void Object::Spin(double angle, const Vector3D& axis) {
    // Implement spin logic
}

}  // namespace Raytracer
