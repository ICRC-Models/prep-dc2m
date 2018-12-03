#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>

// clang++ -O3 -std=c++11 agePop.cpp csvUtil.cpp

// prototype -- move to header eventually
Eigen::MatrixXd readCSV(std::string filename, int cols, int rows);
void writeCSV(Eigen::MatrixXd matrix, std::string filename);

// int pop_cols = 13;
// int pop_rows = 82944;

const double time_step = 0.1; // This needs to be moved out at some point - other functions also rely on it.

const int hivInd = 0;
const int ageInd = 1;
const int maleInd = 2;
const int riskInd = 3;
const int cd4Ind = 4;
const int vlInd = 5;
const int circInd = 6;
const int prepInd = 7;
const int condomInd = 8;
const int artInd = 9;
const int countInd = 10;
const int diffInd = 11;
const int timeInd = 12;


void agePop(Eigen::MatrixXd &pop) {

	const int nHIV = 2;
    const int nAge = 12;
    const int nMale = 2;
    const int nRisk = 3;
    const int nCD4 = 6; // len of art_cov_row
    const int nVl = 6;
    const int nCirc = 2;
    const int nPrep = 2;
    const int nCondom = 2;
    const int nArt = 2; // art is 0 or 1
    const int nPopRows = pop.rows();

	// For use inside the loop
	int ihiv, iage, imale, irisk, icd4, ivl, icirc, iprep, icondom, iart;

	for (int rowInd = 0; rowInd < nPopRows; rowInd ++){

	    ihiv = pop(rowInd, hivInd);
	    iage = pop(rowInd, ageInd) - 1; // 1 indexed fucker
	    imale = pop(rowInd, maleInd);
	    irisk = pop(rowInd, riskInd) - 1; // 1 indexed fucker
	    icd4 = pop(rowInd, cd4Ind);
	    ivl = pop(rowInd, vlInd);
	    icirc = pop(rowInd, circInd);
	    iprep = pop(rowInd, prepInd);
	    icondom = pop(rowInd, condomInd);
	    iart = pop(rowInd, artInd);

	    int rowInd_plus_one;

	    // Efflux: aging out of the compartment. 1/5 of 5-year age groups leave each compartment
	    pop(rowInd, diffInd) -= (0.2 * time_step) * pop(rowInd, countInd);

	    //Influx: add this to compartment of one year older (for all but the last age group). This seems easier in an array.
	    rowInd_plus_one = rowInd + nMale + imale*nRisk + irisk*nCD4 + icd4*nVl + ivl*nCirc + icirc*nCondom * iprep*nCondom + icondom*nArt * iart;
	    rowInd_plus_one = rowInd + nMale*nRisk*nCD4*nVl*nCirc*nCondom*nPrep*nArt;

	    if(iage < (nAge - 1)) {

	    	//std::cout << "iage: " << iage << std::endl;
	    	//std::cout << "rowInd: " << rowInd << std::endl;
	    	//std::cout << "rowInd plus one: " << rowInd_plus_one << std::endl;

	    	pop(rowInd_plus_one, diffInd) += 0.2 * time_step * pop(rowInd, countInd);

	   
	     }
	}

	// std::cout << "head: pop" << pop.block(0,0, 10, 13) << std::endl;
}


// int main() {
//     clock_t tStart;
//     clock_t tEnd;
//     Eigen::MatrixXd pop = readCSV("subtractDeaths.out", pop_cols, pop_rows);
//     tStart = clock();
//     agePop(pop);
//     tEnd = clock();
//     std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
//     writeCSV(pop, "agePop.cout");
// }
