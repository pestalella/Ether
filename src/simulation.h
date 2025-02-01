#pragma once

#include <vector>


class Simulation {
private:
	static constexpr unsigned int FIELD_WIDTH = 100;
	static constexpr unsigned int FIELD_HEIGHT = 100;
	static constexpr unsigned int FIELD_DEPTH = 200;
	static constexpr unsigned int SOURCE_STEPS = 10001;
	static constexpr double gridCellWidth = 1E-3; // in meters
	static constexpr double timeStepLenInSeconds = 1.6678E-12;
	static constexpr double c = 299792458;  // speed of light

//	static const double epsilon_0 = 8.8541878188E-12; // Vacuum permitivity
//	static const double mu_0 = 1.25663706127E-6;  // Vacuum permeability


public:
	Simulation();
	void precomputeSources();
	void doStep();
	void render();
	double computeEnergy();

private:
	void simulationStepPerfectReflectionBoundary();
	void simulationStepPerfectReflectionBoundaryOneWaySource();
	void simulationStepPerfectAbsortionBoundary();

private:
	unsigned int timeStep;
	std::vector<double> mE;
	std::vector<double> mH;
	std::vector<double> Ex;
	std::vector<double> Ey;
	std::vector<double> Ez;
	std::vector<double> Hx;
	std::vector<double> Hy;
	std::vector<double> Hz;
	std::vector<double> gE;
	std::vector<double> gH;
};
