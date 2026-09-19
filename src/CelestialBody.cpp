// CelestialBody.cpp
#include "../include/CelestialBody.hh"
#include <GL/glut.h>
using namespace std;
using namespace DataStructures;

// --- Class constructor ---
CelestialBody::CelestialBody(const string& name, double mass, double radius,
                              const Vector& initialPosition, const Vector& initialVelocity,
                              const Mesh& mesh)
    : name(name), radius(radius),
      transform(initialPosition, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}),
      rigidBody(mass, initialVelocity),
      mesh(mesh) {
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

Transform& CelestialBody::getTransform() {
    return transform;
}

const Transform& CelestialBody::getTransform() const {
    return transform;
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
// Rendering:
// ================================================================
void CelestialBody::render() const {
    glPushMatrix();
    transform.applyGL();
    mesh.render();
    glPopMatrix();
}
