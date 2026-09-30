// PetrovaLine.hh
#ifndef PETROVALINE_HH
#define PETROVALINE_HH

#include "../include/DataStructures.hh"
#include "../include/Mesh.hh"
#include "../include/CelestialBody.hh"
#include <vector>
#include <random>

// "Petrova line" from Project Hail Mary: a stream of astrophages strung along 
// a Bezier curve between a star and a CO2-rich planet. No physics of its own
class PetrovaLine {
    public:
        // ================================================================
        // Constants and type aliases:
        // ================================================================
        using BezierPoints = std::array<DataStructures::Vector, 4>;

        // --- Class constructor ---
        PetrovaLine(CelestialBody* sun, CelestialBody* CO2planet, int resolution, double astrophageSize, float distribution);

        // ================================================================
        // Setters and getters:
        // ================================================================
        // Each regenerates the astrophage cloud
        void setResolution(int resolution);
        void setAstrophageSize(double astrophageSize);
        void setDistribution(float distribution);

        CelestialBody* getSun() const;
        CelestialBody* getCO2Planet() const;
        int getResolution() const;
        double getAstrophageSize() const;
        float getDistribution() const;
        const std::vector<Mesh>& getAstrophages() const;
        const Mesh& getCentralLine() const;
        BezierPoints getBezierPoints() const;

        // ================================================================
        // Simulation:
        // ================================================================
        void advance();

        // ================================================================
        // Rendering:
        // ================================================================
        void render() const;

    private:
        std::vector<Mesh> astrophages;
        Mesh centralLine; // A single thick tube mesh tracing the curve's general direction
        CelestialBody* sun;
        CelestialBody* CO2planet;
        DataStructures::Vector oldCO2planetPosition;
        BezierPoints bezierPoints;
        int resolution;
        double astrophageSize;
        float distribution;

        // --- Central line tube shape ---
        static constexpr int LINE_SEGMENTS = 40; // rings sampled along the curve
        static constexpr int LINE_SIDES = 6;     // vertices per ring (hexagonal cross-section)

        // --- Randomness handlers ---
        std::mt19937 gen;
        std::uniform_real_distribution<double> dis;

        // ================================================================
        // Private methods:
        // ================================================================
        void createAstrophages();
        void createCentralLine();
        DataStructures::Vector calculatePoint(float t, bool isDist);
        BezierPoints calculateBezierPoints();
};
#endif
