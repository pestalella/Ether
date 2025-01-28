#include "simulation.h"

#include <GL/glut.h>
#include <cmath>

Simulation::Simulation() :
	Ex(FIELD_DEPTH, 0),
	Ey(FIELD_DEPTH, 0),
	Ez(FIELD_DEPTH, 0),
	Hx(FIELD_DEPTH, 0),
	Hy(FIELD_DEPTH, 0),
	Hz(FIELD_DEPTH, 0),
	gaussianSource(SOURCE_STEPS, 0)
{
}

void Simulation::precomputeSources()
{
	double tau = timeStepLenInSeconds * 20;
	for (unsigned int i = 0; i < SOURCE_STEPS; ++i) {
		double t = (double)i*timeStepLenInSeconds;
		double x = (t - 7*tau)/tau;
		gaussianSource[i] = std::exp(-(x*x));
	}
}

void Simulation::injectSource(unsigned int timeStep)
{
	Ey[FIELD_DEPTH/2] += gaussianSource[timeStep % SOURCE_STEPS];
	//Hx[FIELD_DEPTH/2] += gaussianSource[timeStep % SOURCE_STEPS];
}

//	static const double epsilon_0 = 8.8541878188E-12; // Vacuum permitivity
//	static const double mu_0 = 1.25663706127E-6;  // Vacuum permeability

void Simulation::simulationStepPerfectReflectionBoundary()
{
	// Update H from E (Dirichlet Boundary Conditions)
	for (unsigned int i = 0; i < FIELD_DEPTH - 1; ++i) {
		Hx[i] = Hx[i] + mH*(Ey[i+1] - Ey[i]);
	}
	Hx[FIELD_DEPTH-1] = Hx[FIELD_DEPTH-1] + mH*(0 - Ey[FIELD_DEPTH-1]);

	// Update H from E (Dirichlet Boundary Conditions)
	Ey[0] = Ey[0] + mE*(Hx[0] - 0);
	for (unsigned int i = 1; i < FIELD_DEPTH - 1; ++i) {
		Ey[i] = Ey[i] + mE*(Hx[i] - Hx[i-1]);
	}
}

void Simulation::simulationStepPerfectAbsortionBoundary()
{
	static double E0 = 0;
	static double E1 = 0;
	static double E2 = 0;
	static double H0 = 0;
	static double H1 = 0;
	static double H2 = 0;

	// Update H from E (Dirichlet Boundary Conditions)
	for (unsigned int i = 0; i < FIELD_DEPTH - 1; ++i) {
		Hx[i] = Hx[i] + mH*(Ey[i+1] - Ey[i]);
	}
	E2 = E1; E1 = E0; E0 = Ey[FIELD_DEPTH-1];
	Hx[FIELD_DEPTH-1] = Hx[FIELD_DEPTH-1] + mH*(E2 - Ey[FIELD_DEPTH-1]);

	// Update H from E (Dirichlet Boundary Conditions)
	H2 = H1; H1 = H0; H0 = Hx[0];
	Ey[0] = Ey[0] + mE*(Hx[0] - H2);
	for (unsigned int i = 1; i < FIELD_DEPTH - 1; ++i) {
		Ey[i] = Ey[i] + mE*(Hx[i] - Hx[i-1]);
	}
}

void Simulation::render()
{
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
}
