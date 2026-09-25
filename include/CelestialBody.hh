// CelestialBody.hh
#ifndef CELESTIALBODY_H
#define CELESTIALBODY_H

#include "../include/RigidBody.hh"
#include "../include/Mesh.hh"
#include <string>

// Represents a single astronomical object (RigidBody + Mesh)
class CelestialBody {
    public:
        // --- Class constructor ---
        CelestialBody(const std::string& name, double mass, double radius,
                      const DataStructures::Vector& initialPosition,
                      const DataStructures::Vector& initialVelocity,
                      const Mesh& mesh);

        // ================================================================
        // Getters:
        // ================================================================
        const std::string& getName() const;
        double getRadius() const; // Physical radius
        DataStructures::Vector getPosition() const; // Forwards to the RigidBody's position

        RigidBody& getRigidBody();
        const RigidBody& getRigidBody() const;
        Mesh& getMesh();
        const Mesh& getMesh() const;

        // ================================================================
        // Simulation:
        // ================================================================
        // Integrates the RigidBody by deltaTime and moves the Mesh's vertices by the
        // resulting change in position, keeping the visual and physical positions in sync.
        void advance(double deltaTime);

        // Applies an axial spin to the mesh, rotated around the body's current position.
        void spin(double pitch, double yaw, double roll);

        // ================================================================
        // Rendering:
        // ================================================================
        void render() const;

    private:
        std::string name;
        double radius;
        RigidBody rigidBody;
        Mesh mesh;
};

#endif
