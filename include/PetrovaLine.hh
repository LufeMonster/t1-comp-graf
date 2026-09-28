// PetrovaLine.hh
#ifndef PETROVALINE_HH
#define PETROVALINE_HH

#include "../include/DataStructures.hh"
#include "../include/Mesh.hh"
#include "../include/CelestialBody.hh"
#include <vector>
#include <random>
class PetrovaLine {
    public:
        // --- Type aliases ---
        using BezierPoints = std::array<DataStructures::Vector, 4>;
        // --- Class constructor ---
        PetrovaLine(CelestialBody* sun, CelestialBody* CO2planet, int resolution, double astrophageSize, float distribution);
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
        CelestialBody* sun;
        CelestialBody* CO2planet;
        DataStructures::Vector oldCO2planetPosition;
        BezierPoints bezierPoints;
        int resolution;
        double astrophageSize;
        float distribution;
        void createAstrophages();
        DataStructures::Vector calculatePoint(float t, bool isDist);
        BezierPoints calculateBezierPoints();
        std::mt19937 gen;
        std::uniform_real_distribution<double> dis;
};
#endif