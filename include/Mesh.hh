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
             const DataStructures::ColorVector& color = DataStructures::WHITE);

        // ================================================================
        // Setters and getters:
        // ================================================================
        void setColor(const DataStructures::ColorVector& color);
        const std::vector<DataStructures::Vector>& getVertices() const;
        const std::vector<Face>& getFaces() const;
        DataStructures::ColorVector getColor() const;

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
        void computeFaceNormals();

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
                                    const DataStructures::ColorVector& color = DataStructures::WHITE);
        // Returns a single closed loop of points - call renderWireframe() to draw it as an orbit path
        static Mesh generateOrbitRing(double radius, int segments,
                                       const DataStructures::ColorVector& color = DataStructures::WHITE);

    private:
        std::vector<DataStructures::Vector> vertices;
        std::vector<Face> faces;
        std::vector<DataStructures::Vector> faceNormals;
        DataStructures::ColorVector color;
};

#endif
