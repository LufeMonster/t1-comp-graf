// PetrovaLine.cpp
#include "../include/PetrovaLine.hh"
#include <math.h>
#include <random>
using namespace std;
using namespace DataStructures;

PetrovaLine::PetrovaLine(CelestialBody* sun, CelestialBody* CO2planet, int resolution, double astrophageSize, float distribution) : 
    sun(sun), CO2planet(CO2planet), resolution(resolution), astrophageSize(astrophageSize), distribution(distribution), gen(random_device{}()), dis(0.0, 1.0) {
    oldCO2planetPosition = CO2planet->getPosition();
    bezierPoints = calculateBezierPoints();
    createAstrophages();
}

void PetrovaLine::createAstrophages() {
    astrophages.clear();
    float increment = 1.0 / (float)resolution;
    Vector position;
    Vector sunPosition = sun->getPosition();
    Vector currCO2planetPosition =  CO2planet->getPosition();
    double sunRadius = sun->getRadius();
    double CO2planetRadius = CO2planet->getRadius();
    double dist = distance(sunPosition, currCO2planetPosition);
    LightVector emission = {RED[0], RED[1], RED[2], 1.0f};

    for (float t = 0.0f; t<=1.05f; t+=increment) {
        position = calculatePoint(t, true);
        if(distance(position, sunPosition) > sunRadius && distance(position, sunPosition) < dist) {
            astrophages.push_back(Mesh::generateSphere(astrophageSize, 3, 3, RED));
            astrophages.back().translate(position);
            astrophages.back().setMaterial(VOID_LIGHT, VOID_LIGHT, VOID_LIGHT, 0.0f);
            astrophages.back().setEmission(emission);
        }
    }
}
void PetrovaLine::advance() {
    Vector sunPosition = sun->getPosition();
    Vector currCO2planetPosition =  CO2planet->getPosition();
    double angle = findAngle3Points(oldCO2planetPosition, sunPosition, currCO2planetPosition);
    oldCO2planetPosition = currCO2planetPosition;
    for(int i = 0; i < astrophages.size(); i++)
        astrophages[i].rotate(sunPosition, 0.0, -angle, 0.0);
}

void PetrovaLine::render() const {
    for(int i = 0; i < astrophages.size(); i++)
        astrophages[i].render();
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