// Camera.cpp
#include "../include/Camera.hh"
#include <cmath>
#include <algorithm>
using namespace std;
using namespace DataStructures;

// --- Class constructor and destructor ---
Camera::Camera(const Vector& position, double pitch, double yaw, double roll, const Vector& up, double fov, double aspectRatio) :
    position(position), up(up), yaw(yaw), roll(roll), fov(fov), aspectRatio(aspectRatio) {

    this->pitch = clamp(pitch, -1.56905, 1.56905);
    this->direction = {cos(this->pitch) * sin(yaw), sin(this->pitch), cos(this->pitch) * cos(yaw)};

    this->nearPlane = 0.1;
    this->farPlane = 4096.0;

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
    this->pitch = clamp(pitch, -1.56905, 1.56905);
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

array<double, 2> Camera::getNearFarPlanes() const {
    return array<double, 2> {nearPlane, farPlane};
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
    Vector translation = normalize({direction[0], 0, direction[2]});
    translation = multiplyVectorScalar(translation, distance);
    position = addVectors(position, translation);
}

void Camera::moveLeftRight(double distance) {
    Vector translation = normalize({direction[2], 0, -direction[0]});
    translation = multiplyVectorScalar(translation, distance);
    position = addVectors(position, translation);
}
    
void Camera::updateRotation(double deltaPitch, double deltaYaw) {
    this->pitch = clamp(this->pitch + deltaPitch, -1.56905, 1.56905); // Apply the rotation change and strictly bound it between -89.9 and 89.9 degrees
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