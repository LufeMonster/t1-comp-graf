// Camera.hh
#ifndef CAMERA_H
#define CAMERA_H

#include "../include/DataStructures.hh"
#include <tuple>

class Camera {
    public:
        // --- Class constructor and destructor ---
        Camera(const DataStructures::Vector& position, double pitch, double yaw, double roll, const DataStructures::Vector& up, double fov, double aspectRatio);
        ~Camera();

        // ================================================================
        // Type aliases:
        // ================================================================
        using initialParametersTuple = std::tuple<DataStructures::Vector, DataStructures::Vector, double, double, double, double, double, double, double>;
            // initialPosition, initialUp, initialPitch, initialYaw, initialRoll, initialFov, initialAspectRatio, initialNearPlane, initialFarPlane

        // ================================================================
        // Setters and getters for camera properties:
        // ================================================================
        // --- Setters ---
        void setPosition(const DataStructures::Vector& pos);
        void setRotation(double pitch, double yaw, double roll);
        void setUp(const DataStructures::Vector& up);
        void setFov(double fov);
        void setAspectRatio(double aspectRatio);
        void setNearFarPlanes(double nearPlane, double farPlane);

        // --- Getters ---
        initialParametersTuple getInitialParameters() const;
        DataStructures::Matrix getCameraParameters() const;
        DataStructures::Vector getPosition() const;
        DataStructures::Vector getDirection() const;
        DataStructures::Vector getUp() const;
        double getFov() const;
        double getAspectRatio() const;

        // ================================================================
        // Camera movement and rotation methods:
        // ================================================================
        void move(const DataStructures::Vector& translation);
        void moveForwardBackward(double distance);
        void moveLeftRight(double distance);
        void updateRotation(double deltaPitch, double deltaYaw);
        //void updateRotation(double deltaPitch, double deltaYaw, double deltaRoll);
        void lookAt(const DataStructures::Vector& target);

    private:
        // ================================================================
        // Camera properties:
        // ================================================================
        DataStructures::Vector position, direction, up;
        double pitch, yaw, roll; // pitch = up/down (X axis), yaw = left/right (Y axis), roll = tilt rotation (Z axis)
        double fov, aspectRatio, nearPlane, farPlane;
        initialParametersTuple initialParameters;
            // initialPosition, initialUp, initialPitch, initialYaw, initialRoll, initialFov, initialAspectRatio, initialNearPlane, initialFarPlane
};

#endif