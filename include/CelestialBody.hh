// CelestialBody.hh
#ifndef CELESTIALBODY_H
#define CELESTIALBODY_H

#include "../include/Transform.hh"
#include "../include/RigidBody.hh"
#include "../include/Mesh.hh"
#include <string>

// Represents a single astronomical object (Sun, planet, moon...) by composing
// a Transform (where it is / how it's oriented), a RigidBody (how it moves)
// and a Mesh (how it's drawn). This mirrors the Transform/Renderer/RigidBody
// component split commonly found in game engines (Unity, Godot) and physics
// engines (Box2D, Bullet), simplified for a single-object-per-entity case.
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
        double getRadius() const; // Physical radius, used for gravity/collision - independent of render scale

        Transform& getTransform();
        const Transform& getTransform() const;
        RigidBody& getRigidBody();
        const RigidBody& getRigidBody() const;
        Mesh& getMesh();
        const Mesh& getMesh() const;

        // ================================================================
        // Rendering:
        // ================================================================
        void render() const;

    private:
        std::string name;
        double radius;
        Transform transform;
        RigidBody rigidBody;
        Mesh mesh;
};

#endif
