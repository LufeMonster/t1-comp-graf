// DataStructures.hh
#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <array>
#include <tuple>
#include <GL/glut.h>

namespace DataStructures {

    // ================================================================
    // Constants and type aliases:
    // ================================================================
    // --- Constants ---
    inline constexpr int DIMENSION = 3;

    // --- Type aliases ---
    using Vector = std::array<double, DIMENSION>;
    using HomogeneousVector = std::array<double, (DIMENSION + 1)>;
    using Matrix = std::array<std::array<double, DIMENSION>, DIMENSION>;
    using HomogeneousMatrix = std::array<std::array<double, (DIMENSION + 1)>, (DIMENSION + 1)>;
    using ColorVector = std::array<float, 3>;

    // ================================================================
    // Function declarations:
    // ================================================================
    Vector normalize(const Vector& v);
    Vector addVectors(const Vector& u, const Vector& v);
    Vector multiplyHMatrixVector(const HomogeneousMatrix& A, const Vector& v);
    HomogeneousMatrix multiplyHMatrices(const HomogeneousMatrix& A, const HomogeneousMatrix& B);
    
    double magnitude(const Vector& v);
    double distance(const Vector& u, const Vector& v);
    void printVector(const Vector& v);
    void printMatrix(const Matrix& A);
    void printHMatrix(const HomogeneousMatrix& A);

    // --- DIMENSION-dependent functions ---
    template <int D = DIMENSION, typename = std::enable_if_t<D == 3>>
    Vector crossProduct(const Vector& u, const Vector& v);

    template <int D, typename>
    Vector crossProduct(const Vector& u, const Vector& v) {
        return {
            u[1] * v[2] - u[2] * v[1],
            u[2] * v[0] - u[0] * v[2],
            u[0] * v[1] - u[1] * v[0]
        };
    }

    // ================================================================
    // OpenGL wrappers:
    // ================================================================
    struct gl {
        // vector and vertex wrappers
        static void normal(const Vector& v) { glNormal3dv(v.data()); };
        static void vertex(const Vector& v) { glVertex3dv(v.data()); };
        static void normal3f(const Vector& v) { glNormal3f(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2]));};
        static void vertex3f(const Vector& v) { glVertex3f(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2]));};

        // color wrappers
        static void color(const ColorVector& c) { glColor3fv(c.data()); };
        static void color3f(const ColorVector& c) { glColor3f(c[0], c[1], c[2]); };
    };    

    // ================================================================
    // Colors:
    // ================================================================
    const ColorVector WHITE = {0.976, 1.0, 0.996}; // #F9FFFE‌
    const ColorVector LIGHT_GRAY = {0.616, 0.616, 0.592}; // #9D9D97
    const ColorVector GRAY = {0.278, 0.31, 0.321}; // #474F52
    const ColorVector BLACK = {0.114, 0.114, 0.129}; // #1D1D21
    const ColorVector BROWN = {0.514, 0.329, 0.196}; // #835432
    const ColorVector RED = {0.69, 0.18, 0.149}; // #B02E26
    const ColorVector ORANGE = {0.976, 0.502, 0.114}; // #F9801D
    const ColorVector YELLOW = {0.996, 0.847, 0.239}; // #FED83D
    const ColorVector GREEN = {0.369, 0.486, 0.086}; // #5E7C16
    const ColorVector CYAN = {0.086, 0.612, 0.612}; // #169C9C
    const ColorVector BLUE = {0.235, 0.267, 0.667}; // #3C44AA
    const ColorVector PURPLE = {0.537, 0.196, 0.721}; // #8932B8
    const ColorVector MAGENTA = {0.78, 0.306, 0.741}; // #C74EBD
    const ColorVector PINK = {0.953, 0.545, 0.667}; // #F38BAA
    const ColorVector LIGHT_BLUE = {0.227, 0.702, 0.855}; // #3AB3DA
    const ColorVector LIME = {0.502, 0.78, 0.121}; // #80C71F
}

#endif