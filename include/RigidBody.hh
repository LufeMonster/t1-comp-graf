// RigidBody.hh
#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "../include/DataStructures.hh"

class RigidBody {
    public:
        // --- Class constructor ---
        RigidBody(double mass = 1.0, const DataStructures::Vector& velocity = {0.0, 0.0, 0.0});

        // ================================================================
        // Setters and getters:
        // ================================================================
        void setMass(double mass);
        void setVelocity(const DataStructures::Vector& velocity);
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
        // integration - more energy-stable than explicit Euler for orbital mechanics.
        // Clears the accumulated force afterwards.
        void integrate(DataStructures::Vector& position, double deltaTime);

    private:
        double mass;
        DataStructures::Vector velocity;
        DataStructures::Vector accumulatedForce;
};

#endif
