// main.cpp
#include "../include/DataStructures.hh"
#include "../include/Camera.hh"
#include "../include/Simulation.hh"
#include "../include/Mesh.hh"
#include "../include/CelestialBody.hh"
#include "../include/PhysicsWorld.hh"
#include <GL/glut.h>
#include <cmath>
#include <iostream>
using namespace std;
using namespace DataStructures;

const Vector initialCameraPosition = {50, 50, 300};
Camera* camera(new Camera(initialCameraPosition, 0.0, 3.14159, 0.0, {0, 1, 0}, 45.0, 1.333)); // 1.5708 3.14159

array<int, 2> windowWidthHeight = {1024, 576};

// ================================================================
// Scene setup:
// ================================================================
// NOTE: these mass/distance/G values are scaled for a visually pleasant simulation,
// not real astronomical units (real values would be either invisibly tiny or huge on screen).
PhysicsWorld* physicsWorld(new PhysicsWorld(/* gravitationalConstant = */ 500.0));
 
CelestialBody* sun(new CelestialBody(
    "Sun", /* mass = */ 20000.0, /* radius = */ 30.0,
    /* position = */ {0.0, 0.0, 0.0},
    /* velocity = */ {0.0, 0.0, 0.0},
    Mesh::generateSphere(30.0, 24, 24, YELLOW)
));
 
CelestialBody* earth(new CelestialBody(
    "Earth", /* mass = */ 100.0, /* radius = */ 8.0,
    /* position = */ {200.0, 0.0, 0.0},
    // Circular-orbit approximation: v = sqrt(G * M_sun / r), tangential to the Sun
    /* velocity = */ {0.0, 0.0, sqrt(500.0 * 20000.0 / 200.0)},
    Mesh::generateSphere(8.0, 16, 16, BLUE)
));


// CelestialBody* moon(new CelestialBody(
//     "Moon", /* mass = */ 1.0, /* radius = */ 2.0,
//     /* position = */ {220.0, 0.0, 0.0}, // 20 units from Earth
//     // Earth's velocity plus the Moon's own orbital velocity around Earth
//     /* velocity = */ {0.0, 0.0, sqrt(500.0 * 20000.0 / 200.0) + sqrt(500.0 * 100.0 / 20.0)},
//     Mesh::generateSphere(2.0, 12, 12, RED)
// ));

Simulation simulation(camera, physicsWorld);

void initOpenGL(void) {
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearColor(LIGHT_GRAY[0], LIGHT_GRAY[1], LIGHT_GRAY[2], 1.0f);

    glEnable(GL_DEPTH_TEST);   // activate the zBuffer

	camera->setFov(45.0);

    // --- Registers the bodies with the physics world ---
    physicsWorld->addBody(sun);
    physicsWorld->addBody(earth);
    //physicsWorld->addBody(moon);
}

array<double, 2> nearFarPlane;
void reshape(int width, int height) {
    
	if (height == 0) height = 1; // Prevents division by zero
	glViewport(0, 0, width, height);
	camera->setAspectRatio((float)width / (float)height);

	glMatrixMode(GL_PROJECTION); // Specifies the projection coordinate system
	glLoadIdentity(); // Initializes coordinate system
    nearFarPlane = camera->getNearFarPlanes();
    gluPerspective(camera->getFov(), camera->getAspectRatio(), nearFarPlane[0], nearFarPlane[1]);

}

void draw(void) {
	// --- Clear screen ---
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// --- Compute camera matrix ---
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // --- Apply camera parameters ---
    Matrix cameraParams = camera->getCameraParameters();
	gluLookAt(cameraParams[0][0], cameraParams[1][0], cameraParams[2][0],  // camera position
			  cameraParams[0][1], cameraParams[1][1], cameraParams[2][1],  // target position
			  cameraParams[0][2], cameraParams[1][2], cameraParams[2][2]); // camera up
    
    // ================================================================
    // Shapes to draw:
    // ================================================================

    physicsWorld->renderAll(); // draws every registered CelestialBody at its current Transform

    glutSwapBuffers();
}

array<int, 2> windowStart = {(1920 - windowWidthHeight[0]) / 2, (1080 - windowWidthHeight[1]) / 2}; // centralizes window for FullHD resolutions
int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidthHeight[0], windowWidthHeight[1]);
    glutInitWindowPosition(windowStart[0], windowStart[1]);
    glutCreateWindow("main");

    glutDisplayFunc(draw);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(Simulation::glutKeyboardCallback);
	glutKeyboardUpFunc(Simulation::glutKeyboardUpCallback);
    //glutSpecialFunc(Simulation::glutSpecialFuncCallback);
	glutMouseFunc(Simulation::glutMouseCallback);
	glutMotionFunc(Simulation::glutMouseMotionControl);
	glutTimerFunc(16, Simulation::glutTimerCallback, 0);

    initOpenGL();
    glutMainLoop();
}