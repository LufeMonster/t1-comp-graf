// RigidBody.cpp
#include "../include/RigidBody.hh"
#include "../include/Transform.hh"
using namespace std;
using namespace DataStructures;

// --- Class constructor ---
RigidBody::RigidBody(const Vector& position, double mass, const Vector& velocity)
    : position(position), mass(mass), velocity(velocity), accumulatedForce({0.0, 0.0, 0.0}) {
}

// ================================================================
// Setters and getters:
// ================================================================
void RigidBody::setPosition(const Vector& position) {
    this->position = position;
}

void RigidBody::setMass(double mass) {
    this->mass = mass;
}

void RigidBody::setVelocity(const Vector& velocity) {
    this->velocity = velocity;
}

Vector RigidBody::getPosition() const {
    return position;
}

double RigidBody::getMass() const {
    return mass;
}

Vector RigidBody::getVelocity() const {
    return velocity;
}

// ================================================================
// Force accumulation:
// ================================================================
void RigidBody::applyForce(const Vector& force) {
    accumulatedForce = addVectors(accumulatedForce, force);
}

void RigidBody::clearForces() {
    accumulatedForce = {0.0, 0.0, 0.0};
}

// ================================================================
// Integration:
// ================================================================
Vector RigidBody::integrate(double deltaTime) {
    // a = F / m
    Vector acceleration = {
        accumulatedForce[0] / mass,
        accumulatedForce[1] / mass,
        accumulatedForce[2] / mass
    };

    // Updates velocity first, then position (semi-implicit Euler)
    velocity[0] += acceleration[0] * deltaTime;
    velocity[1] += acceleration[1] * deltaTime;
    velocity[2] += acceleration[2] * deltaTime;

    Vector deltaPosition = {velocity[0] * deltaTime, velocity[1] * deltaTime, velocity[2] * deltaTime};
    Transform::translate(position, deltaPosition);

    clearForces();
    return deltaPosition;
}
