// SkySphere.hh
#ifndef SKYSPHERE_HH
#define SKYSPHERE_HH

#include "../include/DataStructures.hh"
#include "../include/Mesh.hh"
#include <vector>
#include <random>

// A purely visual backdrop: a field of small, low-polygon, emissive spheres ("stars")
// scattered randomly across the surface of one big sphere centered on the scene. In the
// same spirit as PetrovaLine - just meshes and random scattering, no physics.
class SkySphere {
    public:
        // --- Class constructor ---
        SkySphere(const DataStructures::Vector& center, int starCount, double starSize, double radius);

        // ================================================================
        // Setters and getters:
        // ================================================================
        // Each of these regenerates the star field, since they directly control how it was built.
        void setCenter(const DataStructures::Vector& center);
        void setStarCount(int starCount);
        void setStarSize(double starSize);
        void setRadius(double radius);

        DataStructures::Vector getCenter() const;
        int getStarCount() const;
        double getStarSize() const;
        double getRadius() const;
        const std::vector<Mesh>& getStars() const;

        // ================================================================
        // Rendering:
        // ================================================================
        void render() const;

    private:
        std::vector<Mesh> stars;
        DataStructures::Vector center;
        int starCount;
        double starSize;
        double radius;
        
        // --- Randomness handlers ---
        std::mt19937 gen;
        std::uniform_real_distribution<double> dis;

        // ================================================================
        // Private methods:
        // ================================================================
        void createStars();
        DataStructures::Vector randomPointOnSphere();
};

#endif
