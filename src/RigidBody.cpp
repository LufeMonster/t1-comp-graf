// RigidBody.cpp
#include "../include/RigidBody.hh"
using namespace std;
using namespace DataStructures;

// --- Class constructor ---
RigidBody::RigidBody(double mass, const Vector& velocity)
    : mass(mass), velocity(velocity), accumulatedForce({0.0, 0.0, 0.0}) {
}

// ================================================================
// Setters and getters:
// ================================================================
void RigidBody::setMass(double mass) {
    this->mass = mass;
}

void RigidBody::setVelocity(const Vector& velocity) {
    this->velocity = velocity;
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
void RigidBody::integrate(Vector& position, double deltaTime) {
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

    position[0] += velocity[0] * deltaTime;
    position[1] += velocity[1] * deltaTime;
    position[2] += velocity[2] * deltaTime;

    clearForces();
}
