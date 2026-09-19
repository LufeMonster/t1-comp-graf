// Mesh.hh
#ifndef MESH_H
#define MESH_H

#include "../include/DataStructures.hh"
#include <vector>

class Mesh {
    public:
        // A face is a list of indices into the vertex array (supports triangles, quads and n-gons)
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
        // Normal computation:
        // ================================================================
        // Computes one flat normal per face, from its first three vertices
        void computeFaceNormals();

        // ================================================================
        // Rendering:
        // ================================================================
        void render() const;          // Filled polygons (GL_POLYGON per face)
        void renderWireframe() const; // Line loops per face - used for orbit rings, debugging, etc.

        // ================================================================
        // Primitive factories:
        // ================================================================
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
