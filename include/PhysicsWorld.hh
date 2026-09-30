// PhysicsWorld.hh
#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "../include/CelestialBody.hh"
#include "../include/PetrovaLine.hh"
#include "../include/SkySphere.hh"
#include <vector>

// Holds raw pointers to externally-owned CelestialBody instances, is responsible for computing
// pairwise gravitational forces between them and stepping their motion forward each frame
class PhysicsWorld {
    public:
        // --- Class constructor ---
        PhysicsWorld(double gravitationalConstant = 6.674e-11);

        // ================================================================
        // Body management:
        // ================================================================
        void addBody(CelestialBody* body);
        void setPetrovaLine(PetrovaLine* petrovaLine);
        void setSkySphere(SkySphere* skySphere);
        void setGravitationalConstant(double gravitationalConstant);

        const std::vector<CelestialBody*>& getBodies() const;
        PetrovaLine* getPetrovaLine() const;
        SkySphere* getSkySphere() const;
        double getGravitationalConstant() const;

        // ================================================================
        // Simulation step:
        // ================================================================
        // Computes gravitational forces between every pair of bodies, integrates their
        // motion, and advances the PetrovaLine (if one is attached)
        void step(double deltaTime);

        // ================================================================
        // Rendering:
        // ================================================================
        void renderAll() const;

    private:
        double G; // Gravitational constant
        std::vector<CelestialBody*> bodies;
        PetrovaLine* petrovaLine;
        SkySphere* skySphere;

        void computeGravitationalForces();
};

#endif
