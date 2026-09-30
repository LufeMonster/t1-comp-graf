// Mesh.hh
#ifndef MESH_H
#define MESH_H

#include "../include/DataStructures.hh"
#include <vector>

class Mesh {
    public:
        // A face is a list of indices into the vertex array
        using Face = std::vector<int>;

        // --- Class constructors ---
        Mesh();
        Mesh(const std::vector<DataStructures::Vector>& vertices,
             const std::vector<Face>& faces,
             const DataStructures::ColorVector& color = DataStructures::WHITE,
             bool emissive = false,
             float alpha = 1.0f);

        // ================================================================
        // Setters and getters:
        // ================================================================
        void setColor(const DataStructures::ColorVector& color);
        void setMaterial(float ambientIntensity, float diffuseIntensity, float specularIntensity, float shininess);
        void setEmission(const DataStructures::LightVector& emission);
        // Switches between the two material presets (true = purely self-illuminated)
        void setEmissive(bool emissive);
        void setAlpha(float alpha); // 0.0 = fully transparent, 1.0 = fully opaque
        const std::vector<DataStructures::Vector>& getVertices() const;
        const std::vector<Face>& getFaces() const;
        DataStructures::ColorVector getColor() const;

        DataStructures::LightVector getAmbient() const;
        DataStructures::LightVector getDiffuse() const;
        DataStructures::LightVector getSpecular() const;
        DataStructures::LightVector getEmission() const;
        float getShininess() const;
        bool isEmissive() const;
        float getAlpha() const;

        // ================================================================
        // Visual transformations:
        // ================================================================
        // Rotate/scale recompute face normals afterwards
        void translate(const DataStructures::Vector& translation);
        void rotate(const DataStructures::Vector& rotationCenter, double pitch, double yaw, double roll);
        void scale(const DataStructures::Vector& scaleCenter, double scaleFactor);

        // ================================================================
        // Normal computation:
        // ================================================================
        void computeFaceNormals();   // One flat normal per face
        void computeVertexNormals(); // One normal per vertex, averaged from its adjacent faces

        // ================================================================
        // Rendering:
        // ================================================================
        void render() const;          // Filled polygons (GL_POLYGON per face)
        void renderWireframe() const; // Line loops per face - used for orbit rings, debugging, etc.

        // ================================================================
        // Primitive factories:
        // ================================================================
        // Generated centered at the origin - translate() the result to place it in the scene.
        static Mesh generateSphere(double radius, int stacks, int slices,
                                    const DataStructures::ColorVector& color = DataStructures::WHITE,
                                    bool emissive = false, float alpha = 1.0f);
        // Returns a single closed loop of points - call renderWireframe() to draw it as an orbit path
        static Mesh generateOrbitRing(double radius, int segments,
                                       const DataStructures::ColorVector& color = DataStructures::WHITE,
                                       bool emissive = false, float alpha = 1.0f);
        // Builds a tube following an arbitrary polyline: a ring of 'sides' vertices is
        // placed around each point in 'centerPoints', in the plane perpendicular to the
        // curve's local tangent there, then consecutive rings are connected into quad
        // faces. Needs at least 2 center points; already in world space (no translate()
        // needed afterwards, unlike the other factories).
        static Mesh generateTube(const std::vector<DataStructures::Vector>& centerPoints, double radius, int sides,
                                  const DataStructures::ColorVector& color = DataStructures::WHITE,
                                  bool emissive = false, float alpha = 1.0f);

    private:
        std::vector<DataStructures::Vector> vertices;
        std::vector<Face> faces;
        std::vector<DataStructures::Vector> faceNormals;
        std::vector<DataStructures::Vector> vertexNormals;
        DataStructures::ColorVector color;
        DataStructures::LightVector extendedColor;

        DataStructures::LightVector ambient;
        DataStructures::LightVector diffuse;
        DataStructures::LightVector specular;
        DataStructures::LightVector emission; // Light the mesh emits on its own (GEMISSION), used for self-illuminated objects, such as a star
        float shininess;
        bool emissive;
        float alpha;
};

#endif
