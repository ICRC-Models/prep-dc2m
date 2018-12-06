#include <iostream>
#include <time.h>
#include <eigen3/Eigen/Dense>

// prototype -- move to header eventually
Eigen::MatrixXd readCSV(std::string filename, int cols, int rows);
void writeCSV(Eigen::MatrixXd matrix, std::string filename);
void distributeART(Eigen::MatrixXd &pop, int time_index);
void distributeCondoms(Eigen::MatrixXd &pop, int time_index);
void addBirths(Eigen::MatrixXd &pop, int time_index);
void subtractDeaths(Eigen::MatrixXd &pop, int time_index);
void agePop(Eigen::MatrixXd &pop);
void progressDisease(Eigen::MatrixXd &pop);
void calcMixMat(Eigen::MatrixXd &pop, int time_index);
void adjustPartnerships();
void calcLambda(Eigen::MatrixXd &pop);
void transmit(Eigen::MatrixXd &pop);
void endPop(Eigen::MatrixXd &pop);
void riskAdjust(Eigen::MatrixXd &pop);


int main() {

	// Parameters
	int nSteps = 410;

    int pop_cols = 13;
    int pop_rows = 82944;
    Eigen::MatrixXd pop;

    clock_t tStart;
    clock_t tEnd;

    pop = readCSV("incrt.out", pop_cols, pop_rows);

    // Save initial pop
    writeCSV(pop, "pop_initial.cout");

    std::cout << "Starting Loop..." << std::endl;

	tStart = clock();
	
	// Loop over time steps
	for(int timeIndex = 0; timeIndex < nSteps; timeIndex++) {

		distributeART(pop, timeIndex);

		distributeCondoms(pop, timeIndex);

		addBirths(pop, timeIndex); 

		subtractDeaths(pop, timeIndex); 

		agePop(pop);
	
		progressDisease(pop);

		calcMixMat(pop, timeIndex);

		adjustPartnerships();

		calcLambda(pop);

		transmit(pop);

		endPop(pop);

		riskAdjust(pop);

		
	}

	tEnd = clock();

	std::cout << "Loop Finished." << std::endl;
	std::cout << nSteps << " steps took " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;

	// Save final pop
	writeCSV(pop, "pop_final.cout");


}




