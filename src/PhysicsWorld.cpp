// PhysicsWorld.cpp
#include "../include/PhysicsWorld.hh"
using namespace std;
using namespace DataStructures;

// --- Class constructor ---
PhysicsWorld::PhysicsWorld(double gravitationalConstant)
    : G(gravitationalConstant), petrovaLine(nullptr), skySphere(nullptr) {
}

// ================================================================
// Body management:
// ================================================================
void PhysicsWorld::addBody(CelestialBody* body) {
    bodies.push_back(body);
}

void PhysicsWorld::setPetrovaLine(PetrovaLine* petrovaLine) {
    this->petrovaLine = petrovaLine;
}

void PhysicsWorld::setSkySphere(SkySphere* skySphere) {
    this->skySphere = skySphere;
}

void PhysicsWorld::setGravitationalConstant(double gravitationalConstant) {
    G = gravitationalConstant;
}

const vector<CelestialBody*>& PhysicsWorld::getBodies() const {
    return bodies;
}

PetrovaLine* PhysicsWorld::getPetrovaLine() const {
    return petrovaLine;
}

SkySphere* PhysicsWorld::getSkySphere() const {
    return skySphere;
}

double PhysicsWorld::getGravitationalConstant() const {
    return G;
}

// ================================================================
// Simulation step:
// ================================================================
void PhysicsWorld::step(double deltaTime) {
    computeGravitationalForces();

    for (CelestialBody* body : bodies) {
        body->advance(deltaTime);
    }

    if (petrovaLine != nullptr) {
        petrovaLine->advance();
    }
}

void PhysicsWorld::computeGravitationalForces() {
    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            CelestialBody* a = bodies[i];
            CelestialBody* b = bodies[j];

            Vector posA = a->getPosition();
            Vector posB = b->getPosition();

            Vector direction = {posB[0] - posA[0], posB[1] - posA[1], posB[2] - posA[2]};
            double r = magnitude(direction);

            // Prevents division by zero / extreme forces at very small distances
            if (r < 1e-6) continue;

            Vector unitDirection = normalize(direction);
            double forceMagnitude = G * a->getRigidBody().getMass() * b->getRigidBody().getMass() / (r * r);

            Vector forceOnA = {
                unitDirection[0] * forceMagnitude,
                unitDirection[1] * forceMagnitude,
                unitDirection[2] * forceMagnitude
            };
            Vector forceOnB = {-forceOnA[0], -forceOnA[1], -forceOnA[2]};

            a->getRigidBody().applyForce(forceOnA);
            b->getRigidBody().applyForce(forceOnB);
        }
    }
}

// ================================================================
// Rendering:
// ================================================================
void PhysicsWorld::renderAll() const {
    if (skySphere != nullptr) {
        skySphere->render(); // Drawn first, as a distant backdrop behind everything else
    }

    for (const CelestialBody* body : bodies) {
        body->render();
    }

    if (petrovaLine != nullptr) {
        petrovaLine->render();
    }
}
