// DataStructures.cpp
#include "../include/DataStructures.hh"
#include <array>
#include <cmath>
#include <iostream>
using namespace std;

namespace DataStructures {

    // ================================================================
    // Function implementations:
    // ================================================================
    Vector normalize(const Vector& v) {
        double mag = magnitude(v);

        // Prevents division by zero
        if (mag < 1e-9) { 
            return Vector{};
        }

        Vector result;
        for (int i = 0; i < DIMENSION; ++i) {
            result[i] = v[i] / mag;
        }

        return result;
    }

    Vector multiplyVectorScalar(const Vector& v, double scalar) {
        Vector result;
        
        for (int i = 0; i < DIMENSION; ++i) {
            result[i] = v[i] * scalar;
        }
        
        return result;
    }
    
    Vector addVectors(const Vector& u, const Vector& v) {
        Vector result{};

        for (int i = 0; i < DIMENSION; ++i) {
            result[i] = u[i] + v[i];
        }

        return result;
    }

    Vector multiplyHMatrixVector(const HomogeneousMatrix& A, const Vector& v) {
        HomogeneousVector HVector = {v[0], v[1], v[2], 1.0}; // Convert to homogeneous coordinates
        HomogeneousVector result{};

        for (int i = 0; i < (DIMENSION + 1); ++i) {
            for (int j = 0; j < (DIMENSION + 1); ++j) {
                result[i] += A[i][j] * HVector[j];
            }
        }
        return {result[0], result[1], result[2]}; // Convert back to 3D coordinates
    }

    HomogeneousMatrix multiplyHMatrices(const HomogeneousMatrix& A, const HomogeneousMatrix& B) {
        HomogeneousMatrix result{};

        for (int i = 0; i < (DIMENSION + 1); ++i) {
            for (int j = 0; j < (DIMENSION + 1); ++j) {
                for (int k = 0; k < (DIMENSION + 1); ++k) {
                    result[i][j] += A[i][k] * B[k][j];
                }
            }
        }

        return result;
    }
    
    double magnitude(const Vector& v) {
        double sumOfSquares = 0.0;
        for (int i = 0; i < DIMENSION; ++i) {
            sumOfSquares += v[i] * v[i];
        }
        return sqrt(sumOfSquares);
    }

    double distance(const Vector& u, const Vector& v) {
        Vector result{};
        
        for (int i = 0; i < DIMENSION; ++i) {
            result[i] = u[i] - v[i];
        }
        
        return magnitude(result);
    }

    void printVector(const Vector& v) {
        cout << "[";
        for (int i = 0; i < DIMENSION; ++i) {
            cout << v[i];
            if (i < DIMENSION - 1) {
                cout << ", ";
            }
        }
        cout << "]" << endl;
    }

    void printMatrix(const Matrix& A) {
        for (int i = 0; i < DIMENSION; ++i) {
            cout << "  [";
            for (int j = 0; j < DIMENSION; ++j) {
                cout << A[i][j];
                if (j < DIMENSION - 1) {
                    cout << ", ";
                }
            }
            cout << "]" << endl;
        }
    }

    void printHMatrix(const HomogeneousMatrix& A) {
        for (int i = 0; i < (DIMENSION + 1); ++i) {
            cout << "  [";
            for (int j = 0; j < (DIMENSION + 1); ++j) {
                cout << A[i][j];
                if (j < (DIMENSION + 1) - 1) {
                    cout << ", ";
                }
            }
            cout << "]" << endl;
        };
    }
}
/*
Vector multiplyVectorMatrix(const Vector& v, const HomogeneousMatrix& A) {
    HomogeneousVector HVector = {v[0], v[1], v[2], 1.0}; // Convert to homogeneous coordinates
    HomogeneousVector result{};

    for (int j = 0; j < (DIMENSION + 1); ++j) {
        for (int i = 0; i < (DIMENSION + 1); ++i) {
            result[j] += HVector[i] * A[i][j]; // Traverses down the matrix column
        }
    }
    return {result[0], result[1], result[2]}; // Convert back to 3D coordinates
}
*/