// Simulation.cpp
#include "../include/Simulation.hh"
#include <GL/glut.h>
#include <iostream>

using namespace std;
using namespace DataStructures;

Simulation* Simulation::instance = nullptr;

// --- Class constructor and destructor ---
Simulation::Simulation(Camera* camera) : 
    camera(camera), lastMouseX(0), lastMouseY(0), isMousePressed(false), isShiftPressed(false), previousTime(0), deltaTime(0.0) {

    instance = this;
    for(int i = 0; i < 256; i++) keyStates[i] = false;
    
    // Initializes the time with system time when the object is created
    previousTime = glutGet(GLUT_ELAPSED_TIME);
}

Simulation::~Simulation() {
    if (instance == this)
        instance = nullptr;
}

// ================================================================
// Simulation:
// ================================================================
void Simulation::updateSimulation() {
    // --- Time control ---
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    deltaTime = (currentTime - previousTime) / 1000.0; // in seconds
    previousTime = currentTime;
    if (deltaTime > 0.1) deltaTime = 0.1; // Protection to prevent anomalous spikes (e.g., if the window is dragged and pauses)

    // --- Smooth camera movement ---
    // Scale speed based on SHIFT press
    double speedValue = isShiftPressed ? 60.0 : 20.0;
    double translationSpeed = speedValue * deltaTime;

    if (keyStates['w']) camera->moveForwardBackward(translationSpeed);
    if (keyStates['s']) camera->moveForwardBackward(-translationSpeed);
    if (keyStates['a']) camera->moveLeftRight(translationSpeed);
    if (keyStates['d']) camera->moveLeftRight(-translationSpeed);
    if (keyStates[' ']) camera->move({0.0, translationSpeed, 0.0});
    if (keyStates['c']) camera->move({0.0, -translationSpeed, 0.0});

    // --- Physics simulation ---
    // Always pass 'deltaTime' to your physics functions! 
    // Example:
    // object->updatePosition(objectVelocity * deltaTime); 
    // applyGravity(deltaTime);

    glutPostRedisplay();
}

// ================================================================
// Keyboard & mouse handling:
// ================================================================
void Simulation::keyboardControl(unsigned char key, int x, int y) {
    keyStates[tolower(key)] = true;

    // Check SHIFT state
    int modifiers = glutGetModifiers();
    isShiftPressed = (modifiers & GLUT_ACTIVE_SHIFT);

    if (key == 'r') {
        auto initialParameters = camera->getInitialParameters();
        camera->setPosition(get<0>(initialParameters));
        camera->setUp(get<1>(initialParameters));
        camera->setRotation(get<2>(initialParameters), get<3>(initialParameters), get<4>(initialParameters));
        camera->setFov(get<5>(initialParameters));
        camera->setAspectRatio(get<6>(initialParameters));
        camera->setNearFarPlanes(get<7>(initialParameters), get<8>(initialParameters));
    }
}

void Simulation::keyboardControlUp(unsigned char key, int x, int y) {
    keyStates[tolower(key)] = false;
    
    // Updates SHIFT state after releasing any key
    int modifiers = glutGetModifiers();
    isShiftPressed = (modifiers & GLUT_ACTIVE_SHIFT);
}

void Simulation::mouseControl(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isMousePressed = true;
            lastMouseX = x;
            lastMouseY = y;
            
            int modifiers = glutGetModifiers();
            isShiftPressed = (modifiers & GLUT_ACTIVE_SHIFT);
        } else if (state == GLUT_UP) {
            isMousePressed = false;
        }
    }
}

void Simulation::mouseMotionControl(int x, int y) {
    if (!isMousePressed) return;

    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;
    lastMouseX = x;
    lastMouseY = y;

    //double baseRotationSpeed = isShiftPressed ? 3.0 : 0.8; 
    double baseRotationSpeed = 0.8;
    double rotationSpeed = baseRotationSpeed * deltaTime;

    camera->updateRotation(-deltaY * rotationSpeed, -deltaX * rotationSpeed);
}