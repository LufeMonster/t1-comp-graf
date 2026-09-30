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
    centralLine.rotate(sunPosition, 0.0, -angle, 0.0);
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

    double lineRadius = astrophageSize * 1.5;

    centralLine = Mesh::generateTube(centers, lineRadius, LINE_SIDES, MAGENTA, /* emissive = */ true, /* alpha = */ 0.35f);
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
        addVectors(sunPosition, {(dist * 0.333), 0.0, (dist * -0.25)}),
        addVectors(sunPosition, {(dist * 0.667), 0.0, (dist * -0.25)}),
        CO2planetPosition
    };
    return bezierPoints;
}
