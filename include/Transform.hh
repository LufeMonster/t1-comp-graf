// Transform.hh
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../include/DataStructures.hh"

class Transform {
    public:
        // --- Class constructor ---
        Transform(const DataStructures::Vector& position = {0.0, 0.0, 0.0},
                  const DataStructures::Vector& rotation = {0.0, 0.0, 0.0},
                  const DataStructures::Vector& scale = {1.0, 1.0, 1.0});

        // ================================================================
        // Setters and getters:
        // ================================================================
        void setPosition(const DataStructures::Vector& position);
        void setRotation(const DataStructures::Vector& rotation); // pitch (X), yaw (Y), roll (Z), in radians
        void setScale(const DataStructures::Vector& scale);

        DataStructures::Vector getPosition() const;
        DataStructures::Vector getRotation() const;
        DataStructures::Vector getScale() const;

        // ================================================================
        // Transform operations:
        // ================================================================
        void translate(const DataStructures::Vector& delta);
        void rotate(const DataStructures::Vector& delta);
        void scaleBy(const DataStructures::Vector& factor);

        // ================================================================
        // Matrix composition:
        // ================================================================
        // Builds the model matrix as Translation * Rotation * Scale
        DataStructures::HomogeneousMatrix getModelMatrix() const;

        // ================================================================
        // OpenGL application:
        // ================================================================
        // Multiplies the current OpenGL matrix by this transform's model matrix.
        // Should be wrapped between glPushMatrix()/glPopMatrix() by the caller.
        void applyGL() const;

    private:
        DataStructures::Vector position;
        DataStructures::Vector rotation; // pitch, yaw, roll (radians)
        DataStructures::Vector scale;
};

#endif
