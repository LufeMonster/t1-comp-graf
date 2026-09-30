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

    if (name == "Earth") {

    vector<Vector> continentVertices = {
        {radius + 0.1,  3.0, -2.0},
        {radius + 0.1,  4.0,  0.0},
        {radius + 0.1,  2.5,  2.0},
        {radius + 0.1,  0.5,  2.5},
        {radius + 0.1, -2.0,  1.0},
        {radius + 0.1, -3.5,  0.0},
        {radius + 0.1, -1.0, -1.5},
        {radius + 0.1,  1.0, -2.5}
    };

    vector<Mesh::Face> continentFaces = {
        {0, 1, 2, 3, 4, 5, 6, 7}
    };

    marker = Mesh(continentVertices, continentFaces, GREEN);

    marker.translate(initialPosition);
}
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

    if (name == "Earth") {
        marker.translate(deltaPosition);
    }
}

void CelestialBody::spin(double pitch, double yaw, double roll) {
    mesh.rotate(rigidBody.getPosition(), pitch, yaw, roll);

    if (name == "Earth") {
        marker.rotate(rigidBody.getPosition(), pitch, yaw, roll);
    }
}
void CelestialBody::scale(double scaleFactor) {
    mesh.scale(rigidBody.getPosition(), scaleFactor);

    if (name == "Earth") {
        marker.scale(rigidBody.getPosition(), scaleFactor);
    }
}

// ================================================================
// Rendering:
// ================================================================
void CelestialBody::render() const {
    mesh.render();

    if (name == "Earth") {
        marker.render();
    }
}
