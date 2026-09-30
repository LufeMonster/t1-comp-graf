// LightSource.cpp
#include "../include/LightSource.hh"
using namespace std;
using namespace DataStructures;

// --- Class constructor ---
LightSource::LightSource(const Vector& position, const LightVector& ambientLight, const LightVector& diffuseLight, const LightVector& specularLight)
    : position(position), ambientLight(ambientLight), diffuseLight(diffuseLight), specularLight(specularLight) {
}

// ================================================================
// Setters and getters:
// ================================================================
void LightSource::setPosition(const Vector& position) {
    this->position = position;
}

void LightSource::setAmbientLight(const LightVector& ambientLight) {
    this->ambientLight = ambientLight;
}

void LightSource::setDiffuseLight(const LightVector& diffuseLight) {
    this->diffuseLight = diffuseLight;
}

void LightSource::setSpecularLight(const LightVector& specularLight) {
    this->specularLight = specularLight;
}

Vector LightSource::getPosition() const {
    return position;
}

LightVector LightSource::getAmbientLight() const {
    return ambientLight;
}

LightVector LightSource::getDiffuseLight() const {
    return diffuseLight;
}

LightVector LightSource::getSpecularLight() const {
    return specularLight;
}

// ================================================================
// Intensity control:
// ================================================================
void LightSource::scaleIntensity(double factor) {
    for (int i = 0; i < 3; ++i) {
        ambientLight[i]  = static_cast<float>(ambientLight[i] * factor);
        diffuseLight[i]  = static_cast<float>(diffuseLight[i] * factor);
        specularLight[i] = static_cast<float>(specularLight[i] * factor);
    }
}

// ================================================================
// OpenGL application:
// ================================================================
void LightSource::apply(GLenum lightUnit) const {
    // w determines if light is positional (w = 1.0) or directional (w = 0.0)
    GLfloat glPosition[4] = {static_cast<float>(position[0]), static_cast<float>(position[1]), static_cast<float>(position[2]), 1.0f};

    glLightfv(lightUnit, GL_POSITION, glPosition);
    glLightfv(lightUnit, GL_AMBIENT, ambientLight.data());
    glLightfv(lightUnit, GL_DIFFUSE, diffuseLight.data());
    glLightfv(lightUnit, GL_SPECULAR, specularLight.data());

    glEnable(GL_LIGHTING);
    glEnable(lightUnit);
}

void LightSource::disable(GLenum lightUnit) {
    glDisable(lightUnit);
}
