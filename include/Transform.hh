// Transform.hh
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../include/DataStructures.hh"
#include <vector>

namespace Transform {

    // ================================================================
    // In-place point transformations:
    // ================================================================
    void translate(DataStructures::Vector& point, const DataStructures::Vector& translation);
    void translate(std::vector<DataStructures::Vector>& points, const DataStructures::Vector& translation);

    void rotate(DataStructures::Vector& point, const DataStructures::Vector& rotationCenter,
                double pitch, double yaw, double roll);
    void rotate(std::vector<DataStructures::Vector>& points, const DataStructures::Vector& rotationCenter,
                double pitch, double yaw, double roll);

    void scale(DataStructures::Vector& point, const DataStructures::Vector& scaleCenter, double scaleFactor);
    void scale(std::vector<DataStructures::Vector>& points, const DataStructures::Vector& scaleCenter, double scaleFactor);

    // ================================================================
    // Matrix builders:
    // ================================================================
    DataStructures::HomogeneousMatrix getTranslationMatrix(const DataStructures::Vector& translation);
    DataStructures::HomogeneousMatrix getRotationMatrix(const DataStructures::Vector& rotationCenter,
                                                          double pitch, double yaw, double roll);
    DataStructures::HomogeneousMatrix getScaleMatrix(const DataStructures::Vector& scaleCenter, double scaleFactor);

    // ================================================================
    // Matrix application:
    // ================================================================
    void applyMatrix(DataStructures::Vector& point, const DataStructures::HomogeneousMatrix& matrix);
    void applyMatrix(std::vector<DataStructures::Vector>& points, const DataStructures::HomogeneousMatrix& matrix);
}

#endif
