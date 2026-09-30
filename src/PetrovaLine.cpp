// PetrovaLine.cpp
#include "../include/PetrovaLine.hh"
#include <math.h>
#include <random>
using namespace std;
using namespace DataStructures;

// --- Class constructor ---
PetrovaLine::PetrovaLine(CelestialBody* sun, CelestialBody* CO2planet, int resolution, double astrophageSize, float distribution) : 
    sun(sun), CO2planet(CO2planet), resolution(resolution), astrophageSize(astrophageSize), distribution(distribution), gen(random_device{}()), dis(-0.5, 0.5) {
    oldCO2planetPosition = CO2planet->getPosition();
    bezierPoints = calculateBezierPoints();
    createAstrophages();
    createCentralLine();
}

// ================================================================
// Setters and getters:
// ================================================================
void PetrovaLine::setResolution(int resolution) {
    this->resolution = resolution;
    createAstrophages();
}

void PetrovaLine::setAstrophageSize(double astrophageSize) {
    this->astrophageSize = astrophageSize;
    createAstrophages();
    createCentralLine(); // The tube's radius scales off astrophageSize too
}

void PetrovaLine::setDistribution(float distribution) {
    this->distribution = distribution;
    createAstrophages();
}

CelestialBody* PetrovaLine::getSun() const {
    return sun;
}

CelestialBody* PetrovaLine::getCO2Planet() const {
    return CO2planet;
}

int PetrovaLine::getResolution() const {
    return resolution;
}

double PetrovaLine::getAstrophageSize() const {
    return astrophageSize;
}

float PetrovaLine::getDistribution() const {
    return distribution;
}

const vector<Mesh>& PetrovaLine::getAstrophages() const {
    return astrophages;
}

const Mesh& PetrovaLine::getCentralLine() const {
    return centralLine;
}

PetrovaLine::BezierPoints PetrovaLine::getBezierPoints() const {
    return bezierPoints;
}

// ================================================================
// Simulation:
// ================================================================
void PetrovaLine::advance() {
    Vector sunPosition = sun->getPosition();
    Vector currCO2planetPosition =  CO2planet->getPosition();
    double angle = findAngle3Points(oldCO2planetPosition, sunPosition, currCO2planetPosition);
    oldCO2planetPosition = currCO2planetPosition;

    for(int i = 0; i < (int)astrophages.size(); i++)
        astrophages[i].rotate(sunPosition, 0.0, -angle, 0.0);
    centralLine.rotate(sunPosition, 0.0, -angle, 0.0); // Keeps the tube glued to the same drift as the astrophages
}

// ================================================================
// Rendering:
// ================================================================
void PetrovaLine::render() const {
    for(int i = 0; i < (int)astrophages.size(); i++)
        astrophages[i].render();
    centralLine.render();
}

// ================================================================
// Private methods:
// ================================================================
void PetrovaLine::createAstrophages() {
    astrophages.clear();
    float increment = 1.0 / (float)resolution;
    Vector position;
    Vector sunPosition = sun->getPosition();
    Vector currCO2planetPosition =  CO2planet->getPosition();
    double sunRadius = sun->getRadius();
    double dist = distance(sunPosition, currCO2planetPosition);

    for (float t = 0.0f; t<=1.05f; t+=increment) {
        position = calculatePoint(t, true);
        if(distance(position, sunPosition) > sunRadius && distance(position, sunPosition) < dist) {
            astrophages.push_back(Mesh::generateSphere(astrophageSize, 3, 3, RED, true));
            astrophages.back().translate(position);
        }
    }
}

void PetrovaLine::createCentralLine() {
    // Samples the pure curve (no random offset) to trace its general direction
    vector<Vector> centers;
    centers.reserve(LINE_SEGMENTS + 1);
    for (int i = 0; i <= LINE_SEGMENTS; ++i) {
        centers.push_back(calculatePoint((float)i / (float)LINE_SEGMENTS, false));
    }

    double lineRadius = astrophageSize * 1.5; // A bit thicker than a single astrophage, so the line reads clearly

    vector<Vector> vertices;
    vertices.reserve((LINE_SEGMENTS + 1) * LINE_SIDES);

    for (int i = 0; i <= LINE_SEGMENTS; ++i) {
        // Tangent via forward (or, on the last ring, backward) difference
        Vector tangent = normalize(i < LINE_SEGMENTS
            ? subVectors(centers[i + 1], centers[i])
            : subVectors(centers[i], centers[i - 1]));

        // Any vector not parallel to the tangent works as a seed for a perpendicular basis
        Vector seed = (fabs(tangent[1]) < 0.99) ? Vector{0.0, 1.0, 0.0} : Vector{1.0, 0.0, 0.0};
        Vector right = normalize(crossProduct(tangent, seed));
        Vector trueUp = normalize(crossProduct(right, tangent));

        for (int s = 0; s < LINE_SIDES; ++s) {
            double angle = 2.0 * M_PI * s / LINE_SIDES;
            Vector offset = {
                (right[0] * cos(angle) + trueUp[0] * sin(angle)) * lineRadius,
                (right[1] * cos(angle) + trueUp[1] * sin(angle)) * lineRadius,
                (right[2] * cos(angle) + trueUp[2] * sin(angle)) * lineRadius
            };
            vertices.push_back(addVectors(centers[i], offset));
        }
    }

    // Connects consecutive rings into quad faces, wrapping around each ring
    vector<Mesh::Face> faces;
    faces.reserve(LINE_SEGMENTS * LINE_SIDES);
    for (int i = 0; i < LINE_SEGMENTS; ++i) {
        for (int s = 0; s < LINE_SIDES; ++s) {
            int current     = i * LINE_SIDES + s;
            int currentNext = i * LINE_SIDES + (s + 1) % LINE_SIDES;
            int next        = (i + 1) * LINE_SIDES + s;
            int nextNext    = (i + 1) * LINE_SIDES + (s + 1) % LINE_SIDES;
            faces.push_back({current, next, nextNext, currentNext});
        }
    }

    centralLine = Mesh(vertices, faces, ORANGE, true); // Emissive, so the line reads clearly along its whole length
}

Vector PetrovaLine::calculatePoint(float t, bool isDist) {
    Vector result;
    result[0] = pow(1.0 - t, 3.0) * bezierPoints[0][0] + 3.0 * t * pow(1.0 - t, 2.0) * bezierPoints[1][0] + 3.0 * pow(t, 2.0) * (1.0 - t) * bezierPoints[2][0] + pow(t, 3.0) * bezierPoints[3][0];
    result[1] = pow(1.0 - t, 3.0) * bezierPoints[0][1] + 3.0 * t * pow(1.0 - t, 2.0) * bezierPoints[1][1] + 3.0 * pow(t, 2.0) * (1.0 - t) * bezierPoints[2][1] + pow(t, 3.0) * bezierPoints[3][1];
    result[2] = pow(1.0 - t, 3.0) * bezierPoints[0][2] + 3.0 * t * pow(1.0 - t, 2.0) * bezierPoints[1][2] + 3.0 * pow(t, 2.0) * (1.0 - t) * bezierPoints[2][2] + pow(t, 3.0) * bezierPoints[3][2];
    if(isDist) {
        result[0] += dis(gen) * distribution;
        result[1] += dis(gen) * distribution;
        result[2] += dis(gen) * distribution;
    }
	return result;
}

PetrovaLine::BezierPoints PetrovaLine::calculateBezierPoints() {
    Vector sunPosition = sun->getPosition();
    Vector CO2planetPosition =  CO2planet->getPosition();
    double dist = distance(sunPosition, CO2planetPosition);
    BezierPoints bezierPoints = {
        sunPosition,
        addVectors(sunPosition, {(dist * 0.333), 0.0, -50.0}),
        addVectors(sunPosition, {(dist * 0.667), 0.0, -50.0}),
        CO2planetPosition
    };
    return bezierPoints;
}
