#include "simulation.h"

#include <glm/glm.hpp>
#include <GL/glut.h>

#include <iostream>
#include <unistd.h>

GLsizei screenWidth = 1024;
GLsizei screenHeight = 1024;

GLuint textureID;

Simulation sim;
bool simulationRunning = false;

/**
 * initialize OpenGL
 * disable unused features
**/
void initGL()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDepthFunc((GL_LEQUAL));
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glGenTextures(1, &textureID);
}

void setUpCamera()
{
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

    float widthScale = screenWidth/1024.0;
    float heightScale = screenHeight/1024.0;

    // set perspective viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(widthScale, widthScale, heightScale, heightScale);
    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// void loadTexture(const float *buffer)
// {
// 	glBindTexture(GL_TEXTURE_2D, textureID);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
// 	glTexImage2D(
// 		GL_TEXTURE_2D,
// 		0,
// 		GL_RGB,
// 		FIELD_WIDTH,
// 		FIELD_HEIGHT,
// 		0,
// 		GL_RGB,
// 		GL_FLOAT,
// 		buffer //unsigned char* described up there
// 	);
// }

void keyboardCB(unsigned char key, int, int)
{
    switch (key) {
        case 'q':
		case 27:  // ESC key
			exit(0);
            break;
		case ' ':   // Spacebar: start/stop sim
			simulationRunning = !simulationRunning;
    }
}

void reshapeCB(int width, int height)
{
    screenWidth = width;
    screenHeight = height;
    setUpCamera();
}

void idleCB()
{
	static int stepCount = 0;
	if (simulationRunning) {
		sim.doStep();
		stepCount += 1;
		if (stepCount % 5 == 0)
			glutPostRedisplay();
	}
}

void displayCB()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(1.0, 0.2, 1.0);

	glTranslatef(0.0f, 0.0f, 0.0f);

	sim.render();

	glutSwapBuffers();
}

/**
 * initialize GLUT for windowing
 **/
int initGLUT(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ACCUM | GLUT_MULTISAMPLE);   // display mode
    glutInitWindowSize(screenWidth, screenHeight);  // window size
    glutInitWindowPosition(100, 100);               // window location

    // finally, create a window with openGL context
    // Window will not displayed until glutMainLoop() is called
    // it returns a unique ID
    int handle = glutCreateWindow(argv[0]);     // param is the title of window

    // register GLUT callback functions
    glutDisplayFunc(displayCB);
    glutIdleFunc(idleCB);
    glutReshapeFunc(reshapeCB);
    glutKeyboardFunc(keyboardCB);
    // glutMouseFunc(mouseCB);
    // glutMotionFunc(mouseMotionCB);

    return handle;
}

void setUp(int argc, char **argv)
{
    // init GLUT and GL
    initGLUT(argc, argv);
    initGL();
    setUpCamera();
}

// void initBuffer(std::vector<float> &buf)
// {
// 	for (unsigned int i = 0; i < FIELD_HEIGHT; ++i) {
// 		for (unsigned int j = 0; j < FIELD_WIDTH; ++j) {
// 			buf[3*(FIELD_WIDTH*i + j) + 0] = (sin(j / 1024.0 * 2 * M_PI*5) + 1.0)*0.5;
// 			buf[3*(FIELD_WIDTH*i + j) + 1] = (sin(i / 1024.0 * 2 * M_PI*6) + 1.0)*0.5;
// 			buf[3*(FIELD_WIDTH*i + j) + 2] = 0;

// 		}
// 	}
// }

int main(int argc, char **argv)
{
    srandom(time(NULL));

    setUp(argc, argv);

	// std::vector<float> imgBuf(1024*1024*3);
	// initBuffer(imgBuf);
	// loadTexture(&imgBuf[0]);
	sim.precomputeSources();
    glutMainLoop();
    return 0;
}
