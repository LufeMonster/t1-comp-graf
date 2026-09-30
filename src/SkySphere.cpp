// SkySphere.cpp
#include "../include/SkySphere.hh"
#include <cmath>
using namespace std;
using namespace DataStructures;

// --- Class constructor ---
SkySphere::SkySphere(const Vector& center, int starCount, double starSize, double radius) :
    center(center), starCount(starCount), starSize(starSize), radius(radius), gen(random_device{}()), dis(0.0, 1.0) {
    createStars();
}

// ================================================================
// Setters and getters:
// ================================================================
void SkySphere::setCenter(const Vector& center) {
    this->center = center;
    createStars();
}

void SkySphere::setStarCount(int starCount) {
    this->starCount = starCount;
    createStars();
}

void SkySphere::setStarSize(double starSize) {
    this->starSize = starSize;
    createStars();
}

void SkySphere::setRadius(double radius) {
    this->radius = radius;
    createStars();
}

Vector SkySphere::getCenter() const {
    return center;
}

int SkySphere::getStarCount() const {
    return starCount;
}

double SkySphere::getStarSize() const {
    return starSize;
}

double SkySphere::getRadius() const {
    return radius;
}

const vector<Mesh>& SkySphere::getStars() const {
    return stars;
}

// ================================================================
// Rendering:
// ================================================================
void SkySphere::render() const {
    for (const Mesh& star : stars) {
        star.render();
    }
}

// ================================================================
// Private methods:
// ================================================================
void SkySphere::createStars() {
    stars.clear();
    stars.reserve(starCount);

    for (int i = 0; i < starCount; ++i) {
        Mesh star = Mesh::generateSphere(starSize, 3, 3, WHITE, /* emissive = */ true);
        star.translate(randomPointOnSphere());
        stars.push_back(star);
    }
}

Vector SkySphere::randomPointOnSphere() {
    // Uniform sampling over a sphere's surface: pick z uniformly in [-1, 1] and an
    // angle uniformly in [0, 2*PI] - this avoids the pole-clustering a naive
    // (theta, phi)-both-uniform sampling would produce.
    double theta = 2.0 * M_PI * dis(gen);
    double z = 1.0 - 2.0 * dis(gen);
    double zSquaredComplement = 1.0 - z * z;
    double r = sqrt(zSquaredComplement < 0.0 ? 0.0 : zSquaredComplement);

    Vector direction = {r * cos(theta), z, r * sin(theta)};
    return addVectors(center, multiplyVectorScalar(direction, radius));
}
