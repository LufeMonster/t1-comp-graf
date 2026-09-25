// Transform.cpp
#include "../include/Transform.hh"
#include <cmath>
using namespace std;
using namespace DataStructures;

namespace Transform {

    // ================================================================
    // In-place point transformations:
    // ================================================================
    void translate(Vector& point, const Vector& translation) {
        point = addVectors(point, translation);
    }

    void translate(vector<Vector>& points, const Vector& translation) {
        for (Vector& point : points) {
            translate(point, translation);
        }
    }

    void rotate(Vector& point, const Vector& rotationCenter, double pitch, double yaw, double roll) {
        applyMatrix(point, getRotationMatrix(rotationCenter, pitch, yaw, roll));
    }

    void rotate(vector<Vector>& points, const Vector& rotationCenter, double pitch, double yaw, double roll) {
        applyMatrix(points, getRotationMatrix(rotationCenter, pitch, yaw, roll));
    }

    void scale(Vector& point, const Vector& scaleCenter, double scaleFactor) {
        applyMatrix(point, getScaleMatrix(scaleCenter, scaleFactor));
    }

    void scale(vector<Vector>& points, const Vector& scaleCenter, double scaleFactor) {
        applyMatrix(points, getScaleMatrix(scaleCenter, scaleFactor));
    }

    // ================================================================
    // Matrix builders:
    // ================================================================
    HomogeneousMatrix getTranslationMatrix(const Vector& translation) {
        return {{
            {1.0, 0.0, 0.0, translation[0]},
            {0.0, 1.0, 0.0, translation[1]},
            {0.0, 0.0, 1.0, translation[2]},
            {0.0, 0.0, 0.0, 1.0}
        }};
    }

    HomogeneousMatrix getRotationMatrix(const Vector& rotationCenter, double pitch, double yaw, double roll) {
        HomogeneousMatrix rotationPitch = {{
            {1.0, 0.0, 0.0, 0.0},
            {0.0, cos(pitch), -sin(pitch), 0.0},
            {0.0, sin(pitch), cos(pitch), 0.0},
            {0.0, 0.0, 0.0, 1.0}
        }};
        HomogeneousMatrix rotationYaw = {{
            {cos(yaw), 0.0, sin(yaw), 0.0},
            {0.0, 1.0, 0.0, 0.0},
            {-sin(yaw), 0.0, cos(yaw), 0.0},
            {0.0, 0.0, 0.0, 1.0}
        }};
        HomogeneousMatrix rotationRoll = {{
            {cos(roll), -sin(roll), 0.0, 0.0},
            {sin(roll), cos(roll), 0.0, 0.0},
            {0.0, 0.0, 1.0, 0.0},
            {0.0, 0.0, 0.0, 1.0}
        }};
        HomogeneousMatrix rotation = multiplyHMatrices(rotationRoll, multiplyHMatrices(rotationPitch, rotationYaw));

        // Rotating around a center: move the center to the origin, rotate, move it back
        HomogeneousMatrix toOrigin = getTranslationMatrix({-rotationCenter[0], -rotationCenter[1], -rotationCenter[2]});
        HomogeneousMatrix backToCenter = getTranslationMatrix(rotationCenter);

        return multiplyHMatrices(backToCenter, multiplyHMatrices(rotation, toOrigin));
    }

    HomogeneousMatrix getScaleMatrix(const Vector& scaleCenter, double scaleFactor) {
        HomogeneousMatrix scaleMatrix = {{
            {scaleFactor, 0.0, 0.0, 0.0},
            {0.0, scaleFactor, 0.0, 0.0},
            {0.0, 0.0, scaleFactor, 0.0},
            {0.0, 0.0, 0.0, 1.0}
        }};

        // Scaling around a center: move the center to the origin, scale, move it back
        HomogeneousMatrix toOrigin = getTranslationMatrix({-scaleCenter[0], -scaleCenter[1], -scaleCenter[2]});
        HomogeneousMatrix backToCenter = getTranslationMatrix(scaleCenter);

        return multiplyHMatrices(backToCenter, multiplyHMatrices(scaleMatrix, toOrigin));
    }

    // ================================================================
    // Matrix application:
    // ================================================================
    void applyMatrix(Vector& point, const HomogeneousMatrix& matrix) {
        point = multiplyHMatrixVector(matrix, point);
    }

    void applyMatrix(vector<Vector>& points, const HomogeneousMatrix& matrix) {
        for (Vector& point : points) {
            applyMatrix(point, matrix);
        }
    }
}
