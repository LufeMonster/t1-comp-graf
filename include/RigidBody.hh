// RigidBody.hh
#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "../include/DataStructures.hh"

// The physical representation of a body (position, velocity, forces)
class RigidBody {
    public:
        // --- Class constructor ---
        RigidBody(const DataStructures::Vector& position = {0.0, 0.0, 0.0}, double mass = 1.0, const DataStructures::Vector& velocity = {0.0, 0.0, 0.0});

        // ================================================================
        // Setters and getters:
        // ================================================================
        void setPosition(const DataStructures::Vector& position);
        void setMass(double mass);
        void setVelocity(const DataStructures::Vector& velocity);
        DataStructures::Vector getPosition() const;
        double getMass() const;
        DataStructures::Vector getVelocity() const;

        // ================================================================
        // Force accumulation:
        // ================================================================
        void applyForce(const DataStructures::Vector& force);
        void clearForces();

        // ================================================================
        // Integration:
        // ================================================================
        // Advances velocity and position by deltaTime using semi-implicit (symplectic) Euler
        // integration, clears the accumulated force, and returns the resulting change in
        // position - so the caller can move the visual Mesh by the same amount.
        DataStructures::Vector integrate(double deltaTime);

    private:
        DataStructures::Vector position;
        double mass;
        DataStructures::Vector velocity;
        DataStructures::Vector accumulatedForce;
};

#endif
