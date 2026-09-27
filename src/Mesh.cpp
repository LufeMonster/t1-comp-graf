// Mesh.cpp
#include "../include/Mesh.hh"
#include "../include/Transform.hh"
#include <cmath>
using namespace std;
using namespace DataStructures;

// --- Class constructors ---
Mesh::Mesh() : Mesh({}, {}, WHITE) {
}

Mesh::Mesh(const vector<Vector>& vertices, const vector<Face>& faces, const ColorVector& color)
    : vertices(vertices), faces(faces), color(color),
      // Default material: reacts normally to light, so a freshly-created Mesh is never
      // "transparent" to a light source unless explicitly made so (see setMaterial/setEmission).
      ambient({color[0] * 0.2f, color[1] * 0.2f, color[2] * 0.2f}),
      diffuse({color[0], color[1], color[2], 1.0f}),
      specular({0.3f, 0.3f, 0.3f, 1.0f}),
      emission({0.0f, 0.0f, 0.0f, 1.0f}),
      shininess(32.0f) {
    computeFaceNormals();
}

// ================================================================
// Setters and getters:
// ================================================================
void Mesh::setColor(const ColorVector& color) {
    this->color = color;
}

const vector<Vector>& Mesh::getVertices() const {
    return vertices;
}

const vector<Mesh::Face>& Mesh::getFaces() const {
    return faces;
}

ColorVector Mesh::getColor() const {
    return color;
}

// ================================================================
// Lighting material:
// ================================================================
void Mesh::setMaterial(const LightVector& ambient, const LightVector& diffuse,
                        const LightVector& specular, float shininess) {
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
    this->shininess = shininess;
}

void Mesh::setEmission(const LightVector& emission) {
    this->emission = emission;
}

LightVector Mesh::getAmbient() const {
    return ambient;
}

LightVector Mesh::getDiffuse() const {
    return diffuse;
}

LightVector Mesh::getSpecular() const {
    return specular;
}

LightVector Mesh::getEmission() const {
    return emission;
}

float Mesh::getShininess() const {
    return shininess;
}

// ================================================================
// Visual transformations:
// ================================================================
void Mesh::translate(const Vector& translation) {
    Transform::translate(vertices, translation);
}

void Mesh::rotate(const Vector& rotationCenter, double pitch, double yaw, double roll) {
    Transform::rotate(vertices, rotationCenter, pitch, yaw, roll);
    computeFaceNormals();
}

void Mesh::scale(const Vector& scaleCenter, double scaleFactor) {
    Transform::scale(vertices, scaleCenter, scaleFactor);
    computeFaceNormals();
}

// ================================================================
// Normal computation:
// ================================================================
void Mesh::computeFaceNormals() {
    faceNormals.clear();
    faceNormals.reserve(faces.size());

    for (const Face& face : faces) {
        if (face.size() < 3) {
            faceNormals.push_back({0.0, 0.0, 0.0});
            continue;
        }

        Vector edge1 = {
            vertices[face[1]][0] - vertices[face[0]][0],
            vertices[face[1]][1] - vertices[face[0]][1],
            vertices[face[1]][2] - vertices[face[0]][2]
        };
        Vector edge2 = {
            vertices[face[2]][0] - vertices[face[0]][0],
            vertices[face[2]][1] - vertices[face[0]][1],
            vertices[face[2]][2] - vertices[face[0]][2]
        };

        faceNormals.push_back(normalize(crossProduct(edge1, edge2)));
    }
}

// ================================================================
// Rendering:
// ================================================================
void Mesh::render() const {
    gl::color(color);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient.data());
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse.data());
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular.data());
    glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission.data());

    for (size_t f = 0; f < faces.size(); ++f) {
        const Face& face = faces[f];
        if (face.size() < 3) continue;

        if (f < faceNormals.size()) {
            gl::normal(faceNormals[f]);
        }

        glBegin(GL_POLYGON);
        for (int index : face) {
            gl::vertex(vertices[index]);
        }
        glEnd();
    }
}

void Mesh::renderWireframe() const {
    // Lighting is disabled for wireframes rendering and the re-enabled
    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);

    gl::color(color);
    for (const Face& face : faces) {
        glBegin(GL_LINE_LOOP);
        for (int index : face) {
            gl::vertex(vertices[index]);
        }
        glEnd();
    }

    glPopAttrib();
}

// ================================================================
// Primitive factories:
// ================================================================
Mesh Mesh::generateSphere(double radius, int stacks, int slices, const ColorVector& color) {
    vector<Vector> vertices;
    vector<Face> faces;

    // Generates vertices stack by stack, from the south pole to the north pole
    for (int i = 0; i <= stacks; ++i) {
        double stackAngle = M_PI * i / stacks - M_PI / 2.0; // -PI/2 (south pole) to PI/2 (north pole)
        double xy = radius * cos(stackAngle);
        double y = radius * sin(stackAngle);

        for (int j = 0; j <= slices; ++j) {
            double sliceAngle = 2.0 * M_PI * j / slices;
            vertices.push_back({xy * cos(sliceAngle), y, xy * sin(sliceAngle)});
        }
    }

    int verticesPerStack = slices + 1;
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int current = i * verticesPerStack + j;
            int next = current + verticesPerStack;

            if (i != 0) {
                faces.push_back({current, next, current + 1});
            }
            if (i != (stacks - 1)) {
                faces.push_back({current + 1, next, next + 1});
            }
        }
    }

    return Mesh(vertices, faces, color);
}

Mesh Mesh::generateOrbitRing(double radius, int segments, const ColorVector& color) {
    vector<Vector> vertices;
    Face ring;

    for (int i = 0; i < segments; ++i) {
        double angle = 2.0 * M_PI * i / segments;
        vertices.push_back({radius * cos(angle), 0.0, radius * sin(angle)});
        ring.push_back(i);
    }

    // Stored as a single face; meant to be drawn with renderWireframe(), not render()
    return Mesh(vertices, {ring}, color);
}
