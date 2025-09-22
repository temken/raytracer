#include "Scene/Object.hpp"

namespace Raytracer {

Object::Object(const std::string& name, const Color& color) :
    mName(name),
    mColor(color) {}

Color Object::GetColor() const {
    return mColor;
}

std::string Object::GetName() const {
    return mName;
}

bool Object::EmitsLight() const {
    return mEmitsLight;
}

void Object::SetColor(const Color& color) {
    mColor = color;
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

bool Object::IsVisible() const {
    return mVisible;
}

void Object::SetVisible(bool visible) {
    mVisible = visible;
}

}  // namespace Raytracer
