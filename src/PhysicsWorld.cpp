// PhysicsWorld.cpp
#include "../include/PhysicsWorld.hh"
using namespace std;
using namespace DataStructures;

// --- Class constructor ---
PhysicsWorld::PhysicsWorld(double gravitationalConstant) : G(gravitationalConstant) {
}

// ================================================================
// Body management:
// ================================================================
void PhysicsWorld::addBody(CelestialBody* body) {
    bodies.push_back(body);
}

const vector<CelestialBody*>& PhysicsWorld::getBodies() const {
    return bodies;
}

// ================================================================
// Simulation step:
// ================================================================
void PhysicsWorld::step(double deltaTime) {
    computeGravitationalForces();

    for (CelestialBody* body : bodies) {
        Vector position = body->getTransform().getPosition();
        body->getRigidBody().integrate(position, deltaTime);
        body->getTransform().setPosition(position);
    }
}

void PhysicsWorld::computeGravitationalForces() {
    // Naive O(n^2) pairwise gravity - perfectly fine for a small number of bodies
    // (Sun, Earth, Moon, etc.). Swap for a Barnes-Hut octree if the body count grows large.
    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            CelestialBody* a = bodies[i];
            CelestialBody* b = bodies[j];

            Vector posA = a->getTransform().getPosition();
            Vector posB = b->getTransform().getPosition();

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
    for (const CelestialBody* body : bodies) {
        body->render();
    }
}
