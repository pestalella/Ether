//#include "lens.h"
#include <glm/glm.hpp>
#include <GL/glut.h>

#include <iostream>
#include <vector>

GLsizei screenWidth = 1024;
GLsizei screenHeight = 1024;

const unsigned int IMAGE_WIDTH = 1024;
const unsigned int IMAGE_HEIGHT = 1024;

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
		IMAGE_WIDTH,
		IMAGE_HEIGHT,
		0,
		GL_RGB,
		GL_FLOAT,
		buffer //unsigned char* described up there
	);
}

void displayCB()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glBegin(GL_QUADS);

	glTexCoord2i(0,0); glVertex3f(-0.9f, -0.9f, 0);
	glTexCoord2i(0,1); glVertex3f(0.9f, -0.9f, 0);
	glTexCoord2i(1,1); glVertex3f(0.9f, 0.9f, 0);
	glTexCoord2i(1,0); glVertex3f(-0.9f, 0.9f, 0);

	glEnd();
	glutSwapBuffers();
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
//    glutIdleFunc(updatePopulation);
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
	for (unsigned int i = 0; i < IMAGE_HEIGHT; ++i) {
		for (unsigned int j = 0; j < IMAGE_WIDTH; ++j) {
			buf[3*(IMAGE_WIDTH*i + j) + 0] = (sin(j / 1024.0 * 2 * M_PI*5) + 1.0)*0.5;
			buf[3*(IMAGE_WIDTH*i + j) + 1] = (sin(i / 1024.0 * 2 * M_PI*6) + 1.0)*0.5;
			buf[3*(IMAGE_WIDTH*i + j) + 2] = 0;

		}
	}
}

int main(int argc, char **argv)
{
    srandom(time(NULL));

    setUp(argc, argv);

	std::vector<float> imgBuf(1024*1024*3);
	initBuffer(imgBuf);
	loadTexture(&imgBuf[0]);

    glutMainLoop();
    return 0;
}
