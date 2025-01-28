#pragma once

#include <vector>


class Simulation {
private:
	static constexpr unsigned int FIELD_WIDTH = 100;
	static constexpr unsigned int FIELD_HEIGHT = 100;
	static constexpr unsigned int FIELD_DEPTH = 600;
	static constexpr unsigned int SOURCE_STEPS = 10000;
	static constexpr double gridCellWidth = 1E-3; // 1 millimeter
	static constexpr double timeStepLenInSeconds = 1E-12;  // 1 picosecond
	static constexpr double c = 299792458;  // speed of light
	static constexpr double mE = c*timeStepLenInSeconds/gridCellWidth;
	static constexpr double mH = c*timeStepLenInSeconds/gridCellWidth;

public:
	Simulation();
	void precomputeSources();
	void injectSource(unsigned int timeStep);
	void simulationStepPerfectReflectionBoundary();
	void simulationStepPerfectAbsortionBoundary();
	void render();

private:
	std::vector<double> Ex;
	std::vector<double> Ey;
	std::vector<double> Ez;
	std::vector<double> Hx;
	std::vector<double> Hy;
	std::vector<double> Hz;
	std::vector<double> gaussianSource;

};
