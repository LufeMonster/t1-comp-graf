// LightSource.hh
#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "../include/DataStructures.hh"
#include <GL/glut.h>

class LightSource {
    public:
        // --- Class constructor ---
        LightSource(const DataStructures::Vector& position, const DataStructures::LightVector& ambientLight, const DataStructures::LightVector& diffuseLight, const DataStructures::LightVector& specularLight);

        // ================================================================
        // Setters and getters:
        // ================================================================
        void setPosition(const DataStructures::Vector& position);
        void setAmbientLight(const DataStructures::LightVector& ambientLight);
        void setDiffuseLight(const DataStructures::LightVector& diffuseLight);
        void setSpecularLight(const DataStructures::LightVector& specularLight);

        DataStructures::Vector getPosition() const;
        DataStructures::LightVector getAmbientLight() const;
        DataStructures::LightVector getDiffuseLight() const;
        DataStructures::LightVector getSpecularLight() const;

        // ================================================================
        // Intensity control:
        // ================================================================
        // Multiplies every light component (ambient, diffuse and specular) by the same factor
        void scaleIntensity(double factor);

        // ================================================================
        // OpenGL application:
        // ================================================================
        void apply(GLenum lightUnit = GL_LIGHT0) const;
        static void disable(GLenum lightUnit);

    private:
        DataStructures::Vector position;
        DataStructures::LightVector ambientLight;
        DataStructures::LightVector diffuseLight;
        DataStructures::LightVector specularLight;
};

#endif
