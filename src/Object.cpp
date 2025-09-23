#include "Scene/Object.hpp"

namespace Raytracer {

Object::Object(const std::string& name, const Color& color, const Vector3D& position, const Vector3D& normal) :
    mName(name),
    mColor(color),
    mPosition(position),
    mNormal(normal.Normalized()) {}

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

}

}

}  // namespace Raytracer
