#include <iostream>
#include <time.h>
#include <eigen3/Eigen/Dense>
#include "csvUtil.h"
#include "globals.h"

// prototype -- move to header eventually
void distributeART(int time_index);
void distributeCondoms(int time_index);
void addBirths(int time_index);
void subtractDeaths(int time_index);
void agePop();
void progressDisease();
void calcMixMat(int time_index);
void adjustPartnerships();
void calcLambda();
void transmit();
// void endPop();
void riskAdjust();

// clang++ -O3 -std=c++11 -g runModel.cpp distributeART.cpp distributeCondoms.cpp addBirths.cpp subtractDeaths.cpp agePop.cpp progressDisease.cpp riskAdjust.cpp transmit.cpp csvUtil.cpp globals.cpp

int main() {

	initPop("pop_0.out");
	int nSteps = 410;
	char buffer[50];
	int timeInd = 12;



	for (int iter=0; iter < 100; iter++){
		// Parameters


	    // clock_t tStart;
	    // clock_t tEnd;


	    // std::cout << "Starting Loop..." << std::endl;



		// Save final pop
		// int nPopRows = pop.rows();

		// tStart = clock();

		// Loop over time steps
		for(int timeIndex = 0; timeIndex < nSteps; timeIndex++) {

			// for(int rowInd = 0; rowInd < nPopRows; rowInd++) {

			// 	pop(rowInd, timeInd) = timeIndex;

			// }

			distributeART(timeIndex);

			distributeCondoms(timeIndex);

			addBirths(timeIndex);

			subtractDeaths(timeIndex);

			agePop();

			progressDisease();

			calcMixMat(timeIndex);

			adjustPartnerships();

			calcLambda();

			transmit();

			// endPop();

			riskAdjust();

			// sprintf(buffer, "pop_%i.cout", timeIndex);
			// writeCSV(buffer);



		}

		// tEnd = clock();

		// std::cout << "Loop Finished." << std::endl;
		// std::cout << nSteps << " steps took " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;


	}

	writePop("pop_final.cout", timeInd);
}

