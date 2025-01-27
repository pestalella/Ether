//#include "lens.h"
#include <glm/glm.hpp>
#include <GL/glut.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>

GLsizei screenWidth = 1024;
GLsizei screenHeight = 1024;

const unsigned int FIELD_WIDTH = 100;
const unsigned int FIELD_HEIGHT = 100;
const unsigned int FIELD_DEPTH = 200;

GLuint textureID;

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

    // // enable /disable features
    // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    // glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    // glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // glDisable(GL_DEPTH_TEST);
    // glDisable(GL_LIGHTING);
    // glEnable(GL_MULTISAMPLE);
    // glEnable(GL_LINE_SMOOTH);
    // glDisable(GL_CULL_FACE);
    // glDisable(GL_NORMALIZE);
    // glDisable(GL_COLOR_MATERIAL);

    // glClearColor(0, 0, 0, 0);                   // background color
    // glClearStencil(0);                          // clear stencil buffer

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

void loadTexture(const float *buffer)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		FIELD_WIDTH,
		FIELD_HEIGHT,
		0,
		GL_RGB,
		GL_FLOAT,
		buffer //unsigned char* described up there
	);
}

void keyboardCB(unsigned char key, int, int)
{
    switch (key) {
        case '1':
            break;
        case '2':
            break;
        case 'a':
        case 'A':
            // opticSystem.moveLensElement(selectedLens, -0.1);
            // updateLightRays();
            // glutPostRedisplay();
            break;
        case 'd':
        case 'D':
            // opticSystem.moveLensElement(selectedLens, 0.1);
            // updateLightRays();
            // glutPostRedisplay();
            break;
        case 'g':
        case 'G':
            // changeLensWidth(selectedLens, -0.1);
            // updateLightRays();
            // glutPostRedisplay();
            break;
        case 'h':
        case 'H':
            // changeLensWidth(selectedLens, +0.1);
            // updateLightRays();
            // glutPostRedisplay();
            break;
        default:
            std::cout << "Pressed key code: " << int(key) << std::endl;
    }
}

void reshapeCB(int width, int height)
{
    screenWidth = width;
    screenHeight = height;
    setUpCamera();
}

std::vector<double> Ex(FIELD_DEPTH, 0);
std::vector<double> Ey(FIELD_DEPTH, 0);
std::vector<double> Ez(FIELD_DEPTH, 0);
std::vector<double> Hx(FIELD_DEPTH, 0);
std::vector<double> Hy(FIELD_DEPTH, 0);
std::vector<double> Hz(FIELD_DEPTH, 0);

const double gridCellWidth = 1E-3; // 1 millimeter
const double timeStepLenInSeconds = 1E-12;  // 1 picosecond
const unsigned int SOURCE_STEPS = 10000;
std::vector<double> gaussianSource(SOURCE_STEPS, 0);

void precomputeSources()
{
	double tau = timeStepLenInSeconds * 20;
	for (unsigned int i = 0; i < SOURCE_STEPS; ++i) {
		double t = (double)i*timeStepLenInSeconds;
		double x = (t - 6*tau)/tau;
		gaussianSource[i] = std::exp(-(x*x));
	}
}

void injectSource(unsigned int timeStep)
{
	Ey[FIELD_DEPTH/2] += gaussianSource[timeStep % SOURCE_STEPS];
	Hx[FIELD_DEPTH/2] += gaussianSource[timeStep % SOURCE_STEPS];
}

void simulationStep()
{
	static const double epsilon_0 = 8.8541878188E-12; // Vacuum permitivity
	static const double mu_0 = 1.25663706127E-6;  // Vacuum permeability
	static const double c = 299792458;  // speed of light

	static const double mE = c*timeStepLenInSeconds;
	static const double mH = c*timeStepLenInSeconds;

	// Update H from E (Dirichlet Boundary Conditions)
	for (unsigned int i = 0; i < FIELD_DEPTH - 1; ++i) {
		Hx[i] = Hx[i] + mH*(Ey[i+1] - Ey[i])/gridCellWidth;
	}
	Hx[FIELD_DEPTH-1] = Hx[FIELD_DEPTH-1] + mH*(0 - Ey[FIELD_DEPTH-1])/gridCellWidth;

	// Update H from E (Dirichlet Boundary Conditions)
	Ey[0] = Ey[0] + mE*(Hx[0] - 0)/gridCellWidth;
	for (unsigned int i = 1; i < FIELD_DEPTH - 1; ++i) {
		Ey[i] = Ey[i] + mE*(Hx[i] - Hx[i-1])/gridCellWidth;
	}
}

void idleCB()
{
	static int timeStep = 0;
	std::cout << "Time step " << timeStep << std::endl;

	injectSource(timeStep);
	simulationStep();
	glutPostRedisplay();
	usleep(50000);
	timeStep += 1;

}

void displayCB()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(1.0, 0.2, 1.0);

	glTranslatef(0.0f, 0.0f, 0.0f);

	// Render electric field in red
	glColor3f(1.0, 0, 0);
	glBegin(GL_LINE_STRIP);
	for (unsigned int i = 0; i < FIELD_DEPTH; ++i) {
		glVertex3f(((float)i/FIELD_DEPTH)*1.9 - 0.95, Ey[i], 0);
//		glVertex3f(((float)i/FIELD_DEPTH)*1.9 - 0.95, std::sin(i*3.1415*6/FIELD_DEPTH), 0);
	}
	glEnd();
	// Render magnetic field in blue
	glColor3f(0, 0, 1.0);
	glBegin(GL_LINE_STRIP);
	for (unsigned int i = 0; i < FIELD_DEPTH; ++i) {
		glVertex3f(((float)i/FIELD_DEPTH)*1.9 - 0.95, Hx[i], 0);
	}
	glEnd();
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

void initBuffer(std::vector<float> &buf)
{
	for (unsigned int i = 0; i < FIELD_HEIGHT; ++i) {
		for (unsigned int j = 0; j < FIELD_WIDTH; ++j) {
			buf[3*(FIELD_WIDTH*i + j) + 0] = (sin(j / 1024.0 * 2 * M_PI*5) + 1.0)*0.5;
			buf[3*(FIELD_WIDTH*i + j) + 1] = (sin(i / 1024.0 * 2 * M_PI*6) + 1.0)*0.5;
			buf[3*(FIELD_WIDTH*i + j) + 2] = 0;

		}
	}
}

int main(int argc, char **argv)
{
    srandom(time(NULL));

    setUp(argc, argv);

	// std::vector<float> imgBuf(1024*1024*3);
	// initBuffer(imgBuf);
	// loadTexture(&imgBuf[0]);
	precomputeSources();
    glutMainLoop();
    return 0;
}
