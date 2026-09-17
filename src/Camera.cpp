// Camera.cpp
#include "../include/Camera.hh"
#include <cmath>
using namespace std;
using namespace DataStructures;

// --- Class constructor and destructor ---
Camera::Camera(const Vector& position, double pitch, double yaw, double roll, const Vector& up, double fov, double aspectRatio) {
    this->position = position;
    this->direction = {cos(pitch) * sin(yaw), sin(pitch), cos(pitch) * cos(yaw)};
    this->up = up;

    this->pitch = pitch;
    this->yaw = yaw;
    this->roll = roll;

    this->fov = fov;
    this->aspectRatio = aspectRatio;
    this->nearPlane = 0.1;
    this->farPlane = 100.0;

    this->initialParameters = {position, up, pitch, yaw, roll, fov, aspectRatio, 0.1, 100.0};
}

Camera::~Camera() {
}

// ================================================================
// Setters and getters for camera properties:
// ================================================================
// --- Setters ---
void Camera::setPosition(const Vector& pos) {
    this->position = pos;
}

void Camera::setRotation(double pitch, double yaw, double roll) {
    this->pitch = pitch;
    this->yaw = yaw;
    this->roll = roll;
    direction[0] = cos(pitch) * sin(yaw);
    direction[1] = sin(pitch);
    direction[2] = cos(pitch) * cos(yaw);
}

void Camera::setUp(const Vector& up) {
    this->up = up;
}

void Camera::setFov(double fov) {
    this->fov = fov;
}

void Camera::setAspectRatio(double aspectRatio) {
    this->aspectRatio = aspectRatio;
}

void Camera::setNearFarPlanes(double nearPlane, double farPlane) {
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
}

// --- Getters ---
Camera::initialParametersTuple Camera::getInitialParameters() const{
    return initialParameters;
}

Matrix Camera::getCameraParameters() const {
    Matrix cameraParams{};
    cameraParams[0][0] = position[0];
    cameraParams[1][0] = position[1];
    cameraParams[2][0] = position[2];
    cameraParams[0][1] = direction[0] + position[0];
    cameraParams[1][1] = direction[1] + position[1];
    cameraParams[2][1] = direction[2] + position[2];
    cameraParams[0][2] = up[0];
    cameraParams[1][2] = up[1];
    cameraParams[2][2] = up[2];

    return cameraParams;
}

Vector Camera::getPosition() const {
    return position;
}

Vector Camera::getDirection() const {
    return direction;
}

Vector Camera::getUp() const {
    return up;
}

double Camera::getFov() const {
    return fov;
}

double Camera::getAspectRatio() const {
    return aspectRatio;
}

// ================================================================
// Camera movement and rotation methods:
// ================================================================
void Camera::move(const Vector& translation) {
    position = addVectors(position, translation);
}

void Camera::moveForwardBackward(double distance) {
    Vector translation = {direction[0] * distance, 0, direction[2] * distance};
    position = addVectors(position, translation);
}

void Camera::moveLeftRight(double distance) {
    Vector translation = {direction[2] * distance, 0, -direction[0] * distance};
    position = addVectors(position, translation);
}
    
void Camera::updateRotation(double deltaPitch, double deltaYaw) {
    this->pitch += deltaPitch;
    this->yaw   += deltaYaw;
    direction[0] = cos(this->pitch) * sin(this->yaw);
    direction[1] = sin(this->pitch);
    direction[2] = cos(this->pitch) * cos(this->yaw);
}

void Camera::lookAt(const Vector& target) {
    Vector newDirection = {target[0] - position[0], target[1] - position[1], target[2] - position[2]};
    double length = sqrt(newDirection[0] * newDirection[0] + newDirection[1] * newDirection[1] + newDirection[2] * newDirection[2]);
    direction = {newDirection[0] / length, newDirection[1] / length, newDirection[2] / length};
}

/*
void Camera::updateRotation(double deltaPitch, double deltaYaw, double deltaRoll) {
    HomogeneousMatrix rotationHMatrixPitch = {{
        {1.0, 0.0, 0.0, 0.0},
        {0.0, cos(deltaPitch), -sin(deltaPitch), 0.0},
        {0.0, sin(deltaPitch), cos(deltaPitch), 0.0},
        {0.0, 0.0, 0.0, 1.0}
    }};
    HomogeneousMatrix rotationHMatrixYaw = {{
        {cos(deltaYaw), 0.0, sin(deltaYaw), 0.0},
        {0.0, 1.0, 0.0, 0.0},
        {-sin(deltaYaw), 0.0, cos(deltaYaw), 0.0},
        {0.0, 0.0, 0.0, 1.0}
    }};
    HomogeneousMatrix rotationHMatrixRoll = {{
        {cos(deltaRoll), -sin(deltaRoll), 0.0, 0.0},
        {sin(deltaRoll), cos(deltaRoll), 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    }};
    HomogeneousMatrix rotationHMatrix = multiplyHMatrices(
        rotationHMatrixYaw, multiplyHMatrices(
            rotationHMatrixPitch, rotationHMatrixRoll));
    direction = multiplyHMatrixVector(rotationHMatrix, direction);
}
*/

/*
    this->pitch += deltaPitch;
    this->yaw += deltaYaw;
    this->roll += deltaRoll;

    float tempX = direction[0];
    float tempY = direction[1];
    float tempZ = direction[2];
    direction[0] = tempX * cos(yaw) - tempZ * sin(yaw) + position[0];
    direction[1] = tempY * cos(pitch) - tempZ * sin(pitch) + position[1];
    direction[2] = tempX * sin(yaw) + tempZ * cos(yaw) + position[2];
*/

/*
    HomogeneousMatrix rotationMatrixPitch = {{
        {1.0, 0.0, 0.0, 0.0},
        {0.0, cos(pitch), -sin(pitch), 0.0},
        {0.0, sin(pitch), cos(pitch), 0.0},
        {0.0, 0.0, 0.0, 1.0}
    }};
    HomogeneousMatrix rotationMatrixYaw = {{
        {cos(yaw), 0.0, sin(yaw), 0.0},
        {0.0, 1.0, 0.0, 0.0},
        {-sin(yaw), 0.0, cos(yaw), 0.0},
        {0.0, 0.0, 0.0, 1.0}
    }};
    HomogeneousMatrix rotationMatrix = multiplyHMatrices(
        rotationMatrixYaw, rotationMatrixPitch);
    direction = multiplyHMatrixVector(rotationMatrix, direction);
*/