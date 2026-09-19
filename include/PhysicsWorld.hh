// PhysicsWorld.hh
#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "../include/CelestialBody.hh"
#include <vector>

// Owns no bodies (holds raw pointers to externally-owned CelestialBody instances)
// and is responsible for computing pairwise gravitational forces between them
// and stepping their motion forward each frame - analogous to a "world" or
// "dynamicsWorld" object in physics engines like Box2D or Bullet.
class PhysicsWorld {
    public:
        // --- Class constructor ---
        PhysicsWorld(double gravitationalConstant = 6.674e-11);

        // ================================================================
        // Body management:
        // ================================================================
        void addBody(CelestialBody* body);
        const std::vector<CelestialBody*>& getBodies() const;

        // ================================================================
        // Simulation step:
        // ================================================================
        // Computes gravitational forces between every pair of bodies, then integrates their motion.
        void step(double deltaTime);

        // ================================================================
        // Rendering:
        // ================================================================
        void renderAll() const;

    private:
        double G; // Gravitational constant (scale it to fit the simulation's distance/mass/time units)
        std::vector<CelestialBody*> bodies;

        void computeGravitationalForces();
};

#endif
