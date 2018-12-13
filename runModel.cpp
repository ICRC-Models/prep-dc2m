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
void endPop();
void riskAdjust();

// clang++ -O3 -std=c++11 -g runModel.cpp distributeART.cpp distributeCondoms.cpp addBirths.cpp subtractDeaths.cpp agePop.cpp progressDisease.cpp riskAdjust.cpp transmit.cpp csvUtil.cpp globals.cpp

int main() {

	// initParams(); // evetually will load up all auxilary csvs
	initPop("pop_0.out");
	int nSteps = 410;
	char buffer[50];
	int timeInd = 12;
	bool printOutput = false;

	// Parameters
    clock_t tStart;
    clock_t tEnd;

	tStart = clock();

	// Loop over time steps
	std::stringstream filename;

	for (int timeIndex = 0; timeIndex < nSteps; timeIndex++) {

		distributeART(timeIndex);
		if (printOutput){
			filename << "distributeART_" << timeIndex << ".cout";
			writePop(filename.str(), timeIndex);
			filename.str("");
			filename.clear();
		}

		distributeCondoms(timeIndex);
		if (printOutput){
			filename << "distributeCondoms_" << timeIndex << ".cout";
			writePop(filename.str(), timeIndex);
			filename.str("");
			filename.clear();
		}


		addBirths(timeIndex);
		if (printOutput){
			filename << "addBirths_" << timeIndex << ".cout";
			writePop(filename.str(), timeIndex);
			filename.str("");
			filename.clear();
		}

		subtractDeaths(timeIndex);
		if (printOutput){
			filename << "subtractDeaths_" << timeIndex << ".cout";
			writePop(filename.str(), timeIndex);
			filename.str("");
			filename.clear();
		}

		agePop();
		if (printOutput){
			filename << "agePop_" << timeIndex << ".cout";
			writePop(filename.str(), timeIndex);
			filename.str("");
			filename.clear();
		}

		progressDisease();
		if (printOutput){
			filename << "progressDisease_" << timeIndex << ".cout";
			writePop(filename.str(), timeIndex);
			filename.str("");
			filename.clear();
		}

		calcMixMat(timeIndex);
		adjustPartnerships();
		calcLambda();
		transmit();
		if (printOutput){
			writeMixMat(timeIndex);
			writeAdjustedPartnersMat(timeIndex);
			writeLambdaMat(timeIndex);
			filename << "transmit_" << timeIndex << ".cout";
			writePop(filename.str(), timeIndex);
			filename.str("");
			filename.clear();
		}

		endPop();
		if(printOutput){
			filename << "endPop_" << timeIndex << ".cout";
			writePop(filename.str(), timeIndex);
			filename.str("");
			filename.clear();
		}

		riskAdjust();
		if (printOutput){
			filename << "riskAdjust_" << timeIndex << ".cout";
			writePop(filename.str(), timeIndex);
			filename.str("");
			filename.clear();
		}

	}

	tEnd = clock();

	std::cout << "Loop Finished." << std::endl;
	std::cout << nSteps << " steps took " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;


	writePop("pop_final.cout", timeInd);
}

