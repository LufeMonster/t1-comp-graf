// CelestialBody.cpp
#include "../include/CelestialBody.hh"
using namespace std;
using namespace DataStructures;

// --- Class constructor ---
CelestialBody::CelestialBody(const string& name, double mass, double radius,
                              const Vector& initialPosition, const Vector& initialVelocity,
                              const Mesh& mesh)
    : name(name), radius(radius), rigidBody(initialPosition, mass, initialVelocity), mesh(mesh) {
    // The Mesh is generated centered at the origin, so it's moved to the body's initial position once here, and kept in sync afterwards through advance().
    this->mesh.translate(initialPosition);
}

// ================================================================
// Getters:
// ================================================================
const string& CelestialBody::getName() const {
    return name;
}

double CelestialBody::getRadius() const {
    return radius;
}

Vector CelestialBody::getPosition() const {
    return rigidBody.getPosition();
}

RigidBody& CelestialBody::getRigidBody() {
    return rigidBody;
}

const RigidBody& CelestialBody::getRigidBody() const {
    return rigidBody;
}

Mesh& CelestialBody::getMesh() {
    return mesh;
}

const Mesh& CelestialBody::getMesh() const {
    return mesh;
}

// ================================================================
// Simulation:
// ================================================================
void CelestialBody::advance(double deltaTime) {
    Vector deltaPosition = rigidBody.integrate(deltaTime);
    mesh.translate(deltaPosition);
}

void CelestialBody::spin(double pitch, double yaw, double roll) {
    mesh.rotate(rigidBody.getPosition(), pitch, yaw, roll);
}

// ================================================================
// Rendering:
// ================================================================
void CelestialBody::render() const {
    mesh.render();
}
