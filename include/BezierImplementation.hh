// BezierImplementation.hh
#ifndef BEZIERIMPLEMENTATION_H
#define BEZIERIMPLEMENTATION_H

#include "../include/DataStructures.hh"
#include "../include/Mesh.hh"
#include <vector>
class BezierImplementation {
    public:
        // --- Type aliases ---
        using BezierPoints = std::array<DataStructures::Vector, 4>;
        // --- Class constructor ---
        BezierImplementation(BezierPoints bezierPoints, double resolution);
        // ================================================================
        // Rendering:
        // ================================================================
        void render() const;

        void createPoints();
    private:
        std::vector<Mesh> astrophages;
        BezierPoints bezierPoints;
        double resolution;
        DataStructures::Vector calculatePoint(float t);
};
#endif