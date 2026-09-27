// BezierImplementation.cpp
#include "../include/BezierImplementation.hh"
#include <math.h>
using namespace std;
using namespace DataStructures;

BezierImplementation::BezierImplementation(BezierPoints bezierPoints, double resolution) : 
    bezierPoints(bezierPoints), resolution(resolution) {
    createPoints();
}
void BezierImplementation::createPoints() {
    float increment = 1 / resolution;
    LightVector emission = {MAGENTA[0], MAGENTA[1], MAGENTA[2], 1.0f};

    for (float t=0; t<=1.05; t+=increment) {
        astrophages.push_back(Mesh::generateSphere(5.0, 3, 3, MAGENTA));
        astrophages.back().translate(calculatePoint(t));
        astrophages.back().setMaterial(VOID_LIGHT, VOID_LIGHT, VOID_LIGHT, 0.0f);
        astrophages.back().setEmission(emission);
    }
}

void BezierImplementation::render() const {
    for(int i = 0; i < astrophages.size(); i++)
        astrophages[i].render();
}

Vector BezierImplementation::calculatePoint(float t) {
    Vector result;
    result[0] = pow(1.0 - t, 3.0) * bezierPoints[0][0] + 3.0 * t * pow(1.0 - t, 2.0) * bezierPoints[1][0] + 3.0 * pow(t, 2.0) * (1.0 - t) * bezierPoints[2][0] + pow(t, 3.0) * bezierPoints[3][0];
    result[1] = pow(1.0 - t, 3.0) * bezierPoints[0][1] + 3.0 * t * pow(1.0 - t, 2.0) * bezierPoints[1][1] + 3.0 * pow(t, 2.0) * (1.0 - t) * bezierPoints[2][1] + pow(t, 3.0) * bezierPoints[3][1];
    result[2] = pow(1.0 - t, 3.0) * bezierPoints[0][2] + 3.0 * t * pow(1.0 - t, 2.0) * bezierPoints[1][2] + 3.0 * pow(t, 2.0) * (1.0 - t) * bezierPoints[2][2] + pow(t, 3.0) * bezierPoints[3][2];
	return result;
}