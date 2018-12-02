#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>

// clang++ -O3 -std=c++11 subtractDeaths.cpp csvUtil.cpp

// prototype -- move to header eventually
Eigen::MatrixXd readCSV(std::string filename, int cols, int rows);
void writeCSV(Eigen::MatrixXd matrix, std::string filename);

// int pop_cols = 13;
// int pop_rows = 82944;

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

// back_mort header: age,male, mu
int back_mort_cols = 3;
int back_mort_rows = 24;
Eigen::MatrixXd back_mort_mat = readCSV("back_mort.csv", back_mort_cols, back_mort_rows); 
const int muInd = 2; // Column of back_mort_mat that contains background mortality coefficient (zero indexed)

// hiv_mort header: age, cd4, alpha, art, hiv
int hiv_mort_cols = 5;
int hiv_mort_rows = 144;
Eigen::MatrixXd hiv_mort_mat = readCSV("hiv_mort.csv", hiv_mort_cols, hiv_mort_rows); 
const int alphaInd = 2; // Column of hiv_mort_mat that contains HIV mortality coefficient (zero indexed)



void subtractDeaths(Eigen::MatrixXd &pop, int time_index){

	// std::cout << "Pop :" << pop(0, countInd) << std::endl;


	// Constants for pop frame. Maybe we should move these outside later?
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

    // Set up back_mort array.
    double back_mort[nAge][nMale] = {0};

    for(int ii = 0; ii < nAge; ii++) {
    	for(int jj = 0; jj < nMale; jj++){

    		int rowInd;
    		rowInd = ii * nMale + jj;
    		// std::cout << "ii : " << ii << std::endl;
    		// std::cout << "jj : " << jj << std::endl;
    		// std::cout << "row Ind: " << rowInd << std::endl;
    		// std::cout << "mu: " << back_mort_mat(rowInd, muInd) << std::endl;
    		back_mort[ii][jj] = back_mort_mat(rowInd, muInd); // This line requires back_mort_mat to be ordered by age, cd4, and ART status. 

    	}
    }

	// std::cout << "HERE" <<std::endl;
    // Set up hiv_mort array. Drop HIV column - this only applies to HIV+. Note that this reorders with respect to R output.
    double hiv_mort[nAge][nCD4][nArt] = {0};

    for(int ii = 0; ii < nAge; ii++) {
    	for(int jj = 0; jj < nCD4; jj++){
    		for(int kk = 0; kk < nArt; kk++) {

    			int rowInd;
				rowInd =  ii * nCD4 * nArt + jj * nArt + kk;
	    		// std::cout << "ii : " << ii << std::endl;
	    		// std::cout << "jj : " << jj << std::endl;
	    		// std::cout << "kk : " << kk << std::endl;
	    		// std::cout << "row Ind: " << rowInd << std::endl;
	    		// std::cout << "alpha: " << hiv_mort_mat(rowInd, alphaInd) << std::endl;
	    		hiv_mort[ii][jj][kk] = hiv_mort_mat(rowInd, alphaInd); // This line requires hiv_mort_mat to be ordered by age, cd4, and ART status. 

	    		
    		}
    	}
    }

	// Subtract deaths from pop
	// For use in the loop
  	double mu;
  	double alpha;
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

        double diff = 0; // Tracker for number of deaths

        // Background mortality applies to everyone - find correct multiplier
        mu = back_mort[iage][imale];
        diff += pop(rowInd, countInd) * mu;

        // HIV-specific mortality
        if(ihiv == 1) {
        	alpha = hiv_mort[iage][icd4][iart];
        	diff += pop(rowInd, countInd) * alpha;
        }

	    // Add to diff column of pop
	        pop(rowInd, diffInd) -= diff; 
	}

}

// int main() {
//     clock_t tStart;
//     clock_t tEnd;
//     Eigen::MatrixXd pop = readCSV("addBirths.out", pop_cols, pop_rows);
//     tStart = clock();
//     subtractDeaths(pop, 409);
//     tEnd = clock();
//     std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
//     writeCSV(pop, "subtractDeaths.cout");
// }


