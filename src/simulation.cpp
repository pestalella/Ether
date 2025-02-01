#include "simulation.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <cmath>
#include <iostream>

Simulation::Simulation() :
	timeStep(0),
	mE(FIELD_DEPTH, c*timeStepLenInSeconds/gridCellWidth),
	mH(FIELD_DEPTH, c*timeStepLenInSeconds/gridCellWidth),
	Ex(FIELD_DEPTH, 0),
	Ey(FIELD_DEPTH, 0),
	Ez(FIELD_DEPTH, 0),
	Hx(FIELD_DEPTH, 0),
	Hy(FIELD_DEPTH, 0),
	Hz(FIELD_DEPTH, 0),
	gE(SOURCE_STEPS, 0),
	gH(SOURCE_STEPS, 0)
{
}

void Simulation::precomputeSources()
{
	double tau = timeStepLenInSeconds * 20;
	for (unsigned int i = 0; i < SOURCE_STEPS; ++i) {
		double t = (double)i*timeStepLenInSeconds;
		double x = (t - 7*tau)/tau;
		gE[i] = std::exp(-(x*x));
	}

	for (unsigned int i = 0; i < SOURCE_STEPS; ++i) {
		double t = (double)i*timeStepLenInSeconds + timeStepLenInSeconds/(2*c) + timeStepLenInSeconds/2;
		double x = (t - 7*tau)/tau;
		gH[i] = std::exp(-(x*x));
	}
	// for (unsigned int k = 100; k < 200; ++k) {
	// 	mH[k] = 10*c*timeStepLenInSeconds/gridCellWidth;
	// }
}

void Simulation::doStep()
{
	if (timeStep % 100 == 0)
		std::cout << "Time step " << timeStep << "  energy: " << computeEnergy() << "\r" << std::flush;
	simulationStepPerfectReflectionBoundary();
//  simulationStepPerfectAbsortionBoundary();
//	simulationStepPerfectReflectionBoundaryOneWaySource();
	timeStep += 1;
}

void Simulation::simulationStepPerfectReflectionBoundary()
{
	// Inject source
	if (timeStep < SOURCE_STEPS) {
		Ey[FIELD_DEPTH/2] += gE[timeStep];
//		Hx[FIELD_DEPTH/2] += -gH[timeStep];
	}

	std::vector<double> newHx(FIELD_DEPTH);
	// Update H from E (Dirichlet Boundary Conditions)
	for (unsigned int k = 0; k < FIELD_DEPTH - 1; ++k) {
		newHx[k] = Hx[k] + mH[k]*(Ey[k+1] - Ey[k]);
	}
	newHx[FIELD_DEPTH-1] = Hx[FIELD_DEPTH-1] + mH[FIELD_DEPTH-1]*(0 - Ey[FIELD_DEPTH-1]);
	Hx.swap(newHx);

	// Update H from E (Dirichlet Boundary Conditions)
	Ey[0] = Ey[0] + mE[0]*(Hx[0] - 0);
	for (unsigned int k = 1; k < FIELD_DEPTH; ++k) {
		Ey[k] = Ey[k] + mE[k]*(Hx[k] - Hx[k-1]);
	}
}

void Simulation::simulationStepPerfectReflectionBoundaryOneWaySource()
{
	// Inject source
	if (timeStep < SOURCE_STEPS) {
		Ey[FIELD_DEPTH/2] += gE[timeStep % SOURCE_STEPS];
		Hx[FIELD_DEPTH/2] += -gH[timeStep  % SOURCE_STEPS];
	}

	std::vector<double> newHx(FIELD_DEPTH);
	// Update H from E (Dirichlet Boundary Conditions)
	for (unsigned int k = 0; k < FIELD_DEPTH - 1; ++k) {
		newHx[k] = Hx[k] + mH[k]*(Ey[k+1] - Ey[k]);
	}
	newHx[FIELD_DEPTH-1] = Hx[FIELD_DEPTH-1] + mH[FIELD_DEPTH-1]*(0 - Ey[FIELD_DEPTH-1]);
	if (timeStep < SOURCE_STEPS) {
		double injected = gE[timeStep % SOURCE_STEPS];
		double toRemove = mH[FIELD_DEPTH/2-1]*injected;
		newHx[FIELD_DEPTH/2-1] -= toRemove;
	}

	Hx.swap(newHx);

	// Update H from E (Dirichlet Boundary Conditions)
	Ey[0] = Ey[0] + mE[0]*(Hx[0] - 0);
	for (unsigned int k = 1; k < FIELD_DEPTH; ++k) {
		Ey[k] = Ey[k] + mE[k]*(Hx[k] - Hx[k-1]);
	}
	if (timeStep < SOURCE_STEPS)
		Ey[FIELD_DEPTH/2] +=  mE[FIELD_DEPTH/2]*gH[(timeStep + SOURCE_STEPS-1) % SOURCE_STEPS];
}

void Simulation::simulationStepPerfectAbsortionBoundary()
{
	static double E0 = 0;
	static double E1 = 0;
	static double E2 = 0;
	static double H0 = 0;
	static double H1 = 0;
	static double H2 = 0;

	// Inject source
	if (timeStep < SOURCE_STEPS) {
		Ey[FIELD_DEPTH/2] += gE[timeStep];
//		Hx[FIELD_DEPTH/2] += -gH[timeStep];
	}

	std::vector<double> newHx(FIELD_DEPTH);
	// Update H from E (Dirichlet Boundary Conditions)
	for (unsigned int k = 0; k < FIELD_DEPTH - 1; ++k) {
		newHx[k] = Hx[k] + mH[k]*(Ey[k+1] - Ey[k]);
	}
	E2 = E1; E1 = E0; E0 = Ey[FIELD_DEPTH-1];
	newHx[FIELD_DEPTH-1] = Hx[FIELD_DEPTH-1] + mH[FIELD_DEPTH-1]*(E2 - Ey[FIELD_DEPTH-1]);
	Hx.swap(newHx);

	// Update H from E (Dirichlet Boundary Conditions)
	H2 = H1; H1 = H0; H0 = Hx[0];
	Ey[0] = Ey[0] + mE[0]*(Hx[0] - H2);
	for (unsigned int k = 1; k < FIELD_DEPTH; ++k) {
		Ey[k] = Ey[k] + mE[k]*(Hx[k] - Hx[k-1]);
	}
}

double Simulation::computeEnergy()
{
	double energy = 0;
	// Not really energy, but a constant fraction of it.
	for (unsigned int k = 0; k < FIELD_DEPTH; ++k) {
		energy += Ey[k]*Ey[k] + Hx[k]*Hx[k];
	}
	return energy;
}

void Simulation::render()
{
	// Render electric field in blue
	glColor3f(0, 0, 1.0);
	glBegin(GL_LINE_STRIP);
	for (unsigned int i = 0; i < FIELD_DEPTH; ++i) {
		glVertex3f(((float)i/FIELD_DEPTH)*1.9 - 0.95, Ey[i], 0);
//		glVertex3f(((float)i/FIELD_DEPTH)*1.9 - 0.95, std::sin(i*3.1415*6/FIELD_DEPTH), 0);
	}
	glEnd();
	// Render magnetic field in red
	glColor3f(1.0, 0, 0);
	glBegin(GL_LINE_STRIP);
	for (unsigned int i = 0; i < FIELD_DEPTH; ++i) {
		glVertex3f(((float)i/FIELD_DEPTH)*1.9 - 0.95, Hx[i], 0);
	}
	glEnd();
}
