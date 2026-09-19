// Transform.cpp
#include "../include/Transform.hh"
#include <cmath>
using namespace std;
using namespace DataStructures;

// --- Class constructor ---
Transform::Transform(const Vector& position, const Vector& rotation, const Vector& scale) {
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
}

// ================================================================
// Setters and getters:
// ================================================================
void Transform::setPosition(const Vector& position) {
    this->position = position;
}

void Transform::setRotation(const Vector& rotation) {
    this->rotation = rotation;
}

void Transform::setScale(const Vector& scale) {
    this->scale = scale;
}

Vector Transform::getPosition() const {
    return position;
}

Vector Transform::getRotation() const {
    return rotation;
}

Vector Transform::getScale() const {
    return scale;
}

// ================================================================
// Transform operations:
// ================================================================
void Transform::translate(const Vector& delta) {
    position = addVectors(position, delta);
}

void Transform::rotate(const Vector& delta) {
    rotation = addVectors(rotation, delta);
}

void Transform::scaleBy(const Vector& factor) {
    for (int i = 0; i < DIMENSION; ++i) {
        scale[i] *= factor[i];
    }
}

// ================================================================
// Matrix composition:
// ================================================================
HomogeneousMatrix Transform::getModelMatrix() const {
    HomogeneousMatrix scaleMatrix = {{
        {scale[0], 0.0, 0.0, 0.0},
        {0.0, scale[1], 0.0, 0.0},
        {0.0, 0.0, scale[2], 0.0},
        {0.0, 0.0, 0.0, 1.0}
    }};

    double pitch = rotation[0], yaw = rotation[1], roll = rotation[2];

    HomogeneousMatrix rotationPitch = {{
        {1.0, 0.0, 0.0, 0.0},
        {0.0, cos(pitch), -sin(pitch), 0.0},
        {0.0, sin(pitch), cos(pitch), 0.0},
        {0.0, 0.0, 0.0, 1.0}
    }};
    HomogeneousMatrix rotationYaw = {{
        {cos(yaw), 0.0, sin(yaw), 0.0},
        {0.0, 1.0, 0.0, 0.0},
        {-sin(yaw), 0.0, cos(yaw), 0.0},
        {0.0, 0.0, 0.0, 1.0}
    }};
    HomogeneousMatrix rotationRoll = {{
        {cos(roll), -sin(roll), 0.0, 0.0},
        {sin(roll), cos(roll), 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    }};

    HomogeneousMatrix translationMatrix = {{
        {1.0, 0.0, 0.0, position[0]},
        {0.0, 1.0, 0.0, position[1]},
        {0.0, 0.0, 1.0, position[2]},
        {0.0, 0.0, 0.0, 1.0}
    }};

    // Composition order: Translation * Roll * Pitch * Yaw * Scale
    HomogeneousMatrix rotationMatrix = multiplyHMatrices(rotationRoll, multiplyHMatrices(rotationPitch, rotationYaw));
    return multiplyHMatrices(translationMatrix, multiplyHMatrices(rotationMatrix, scaleMatrix));
}

// ================================================================
// OpenGL application:
// ================================================================
void Transform::applyGL() const {
    HomogeneousMatrix model = getModelMatrix();

    // OpenGL expects a column-major 16-value array
    double glMatrix[16];
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            glMatrix[col * 4 + row] = model[row][col];
        }
    }

    glMultMatrixd(glMatrix);
}
