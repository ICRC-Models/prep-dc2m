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

    pop = readCSV("pop_initial.out", pop_cols, pop_rows);

    std::cout << "Starting Loop..." << std::endl;

    char buffer[50];

	tStart = clock();

	// Save final pop
	int nPopRows = pop.rows();
	int timeInd = 12;

	// Loop over time steps
	for(int timeIndex = 0; timeIndex < nSteps; timeIndex++) {


		for(int rowInd = 0; rowInd < nPopRows; rowInd++) {
	
			pop(rowInd, timeInd) = timeIndex;
	
		}

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


		sprintf(buffer, "pop_%i.cout", timeIndex);
		writeCSV(pop, buffer);

		
	}

	tEnd = clock();

	std::cout << "Loop Finished." << std::endl;
	std::cout << nSteps << " steps took " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;



	writeCSV(pop, "pop_final.cout");


}




