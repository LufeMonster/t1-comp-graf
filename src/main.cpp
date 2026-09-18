// main.cpp
#include "../include/DataStructures.hh"
#include "../include/Camera.hh"
#include "../include/Simulation.hh"
#include <GL/glut.h>
#include <iostream>
using namespace std;
using namespace DataStructures;

const Vector initialCameraPosition = {50, 50, 300};
Camera* camera(new Camera(initialCameraPosition, 0.0, 3.14159, 0.0, {0, 1, 0}, 45.0, 1.333)); // 1.5708 3.14159
Simulation simulation(camera);

array<int, 2> windowWidthHeight = {1024, 576};

void initOpenGL(void) {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);   // activate the zBuffer

	camera->setFov(45.0);
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

    glColor3f(1.0f, 0.5f, 0.0f);
    glPushMatrix();
        //glTranslated(0, 0, 0);
        glutWireTeapot(50.0f);  
	    // glColor3f(0.0f, 0.5f, 0.5f); glutWireSphere(50, 20, 20); glColor3f(0.0f, 0.0f, 1.0f); glutWireCube(50.0);
    glPopMatrix();

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