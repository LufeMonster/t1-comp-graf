// Mesh.cpp
#include "../include/Mesh.hh"
#include "../include/Transform.hh"
#include <cmath>
using namespace std;
using namespace DataStructures;

// --- Class constructors ---
Mesh::Mesh() : Mesh({}, {}, WHITE, false) {
}

Mesh::Mesh(const vector<Vector>& vertices, const vector<Face>& faces, const ColorVector& color, bool emissive)
    : vertices(vertices), faces(faces), color(color) {
    this->extendedColor = {color[0], color[1], color[2], 1.0f};
    setEmissive(emissive); // Sets the default reacts-to-light material, or the self-illuminated one
    computeVertexNormals();
}

// ================================================================
// Setters and getters:
// ================================================================
void Mesh::setColor(const ColorVector& color) {
    this->color = color;
    this->extendedColor = {color[0], color[1], color[2], 1.0f};
    if (emissive) {
        emission = {color[0], color[1], color[2], 1.0f}; // Keeps the glow in sync with the mesh's own color
    }
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
void Mesh::setMaterial(float ambientIntensity, float diffuseIntensity, float specularIntensity, float shininess) {
    this->ambient = {ambientIntensity, ambientIntensity, ambientIntensity, 1.0f};
    this->diffuse = {diffuseIntensity, diffuseIntensity, diffuseIntensity, 1.0f};
    this->specular = {specularIntensity, specularIntensity, specularIntensity, 1.0f};
    this->shininess = shininess;
}

void Mesh::setEmission(const LightVector& emission) {
    this->emission = emission;
}

void Mesh::setEmissive(bool emissive) {
    this->emissive = emissive;

    if (emissive) {
        setMaterial(0.0f, 0.0f, 0.0f, 0.0f);
        emission = {color[0], color[1], color[2], 1.0f};
    } else {
        setMaterial(0.2f, 1.0f, 0.3f, 32.0f);
        emission = VOID_LIGHT;
    }
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

bool Mesh::isEmissive() const {
    return emissive;
}

// ================================================================
// Visual transformations:
// ================================================================
void Mesh::translate(const Vector& translation) {
    Transform::translate(vertices, translation);
}

void Mesh::rotate(const Vector& rotationCenter, double pitch, double yaw, double roll) {
    Transform::rotate(vertices, rotationCenter, pitch, yaw, roll);
    computeVertexNormals();
}

void Mesh::scale(const Vector& scaleCenter, double scaleFactor) {
    Transform::scale(vertices, scaleCenter, scaleFactor);
    computeVertexNormals();
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

void Mesh::computeVertexNormals() {
    computeFaceNormals();

    vertexNormals.assign(vertices.size(), Vector{0.0, 0.0, 0.0});

    // Accumulates each face's normal into every vertex it touches...
    for (size_t f = 0; f < faces.size(); ++f) {
        for (int index : faces[f]) {
            vertexNormals[index] = addVectors(vertexNormals[index], faceNormals[f]);
        }
    }

    // ...then averages
    for (Vector& normal : vertexNormals) {
        normal = normalize(normal);
    }
}

// ================================================================
// Rendering:
// ================================================================
void Mesh::render() const {
    glMaterialfv(GL_FRONT, GL_AMBIENT, hadamardProduct(ambient, extendedColor).data());
    glMaterialfv(GL_FRONT, GL_DIFFUSE, hadamardProduct(diffuse, extendedColor).data());
    glMaterialfv(GL_FRONT, GL_SPECULAR, hadamardProduct(specular, extendedColor).data());
    glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission.data());

    gl::color(color);

    for (size_t f = 0; f < faces.size(); ++f) {
        const Face& face = faces[f];
        if (face.size() < 3) continue;

        glBegin(GL_POLYGON);
        for (int index : face) {
            if (index < (int)vertexNormals.size()) {
                gl::normal(vertexNormals[index]);
            }
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
Mesh Mesh::generateSphere(double radius, int stacks, int slices, const ColorVector& color, bool emissive) {
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

    return Mesh(vertices, faces, color, emissive);
}

Mesh Mesh::generateOrbitRing(double radius, int segments, const ColorVector& color, bool emissive) {
    vector<Vector> vertices;
    Face ring;

    for (int i = 0; i < segments; ++i) {
        double angle = 2.0 * M_PI * i / segments;
        vertices.push_back({radius * cos(angle), 0.0, radius * sin(angle)});
        ring.push_back(i);
    }

    // Stored as a single face; meant to be drawn with renderWireframe(), not render()
    return Mesh(vertices, {ring}, color, emissive);
}
