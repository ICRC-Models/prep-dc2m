#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>


// clang++ -O3 -std=c++11 progressDisease.cpp csvUtil.cpp

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

// Read in CD4 and VL disease progression parameters
int dis_prog_cols = 7;
int dis_prog_rows = 50;
Eigen::MatrixXd dis_prog_mat = readCSV("dis_prog.csv", dis_prog_cols, dis_prog_rows); // male, vl, cd4, cd4_duration, vl_duration, hiv, art
const int cd4_durationInd = 3; // Column of dis_prog_mat that contains mean cd4_duration
const int vl_durationInd = 4; // Column of dis_prog_mat that contains mean vl_duration

void progressDisease(Eigen::MatrixXd &pop) {

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


    // Set up dis_prog arrays - ignore HIV and ART columns. Notice this reorders with respect to R output. This is breaking - indexing is wrong somewhere
    double cd4_prog[nMale][nVl-1][nCD4-1] = {0}; 

    for(int ii = 0; ii < nMale; ii++) {
    	for(int jj = 0; jj < nVl-1; jj++){
    		for(int kk = 0; kk < nCD4-1; kk++) {

    			int rowInd; 

    			// std::cout << "ii: " << ii << std::endl;
    			// std::cout << "jj: " << jj << std::endl;
    			// std::cout << "kk: " << kk << std::endl;

    			rowInd = ii * (nVl - 1)*(nCD4 - 1) + jj * (nCD4-1) + kk;
    			// std:: cout << "rowInd: " << rowInd << std::endl;

    			cd4_prog[ii][jj][kk] = dis_prog_mat(rowInd, cd4_durationInd);  
    		}

    	}
    }

    double vl_prog[nMale][nVl-1][nCD4-1] = {0};

    for(int ii = 0; ii < nMale; ii++) {
    	for(int jj = 0; jj < nVl-1; jj++){
    		for(int kk = 0; kk < nCD4-1; kk++) {

    			int rowInd;

    			// std::cout << "ii: " << ii << std::endl;
    			// std::cout << "jj: " << jj << std::endl;
    			// std::cout << "kk: " << kk << std::endl;

    			rowInd = ii * (nVl - 1)*(nCD4 - 1) + jj * (nCD4-1) + kk;
    			// std:: cout << "rowInd: " << rowInd << std::endl;

    			vl_prog[ii][jj][kk] = dis_prog_mat(rowInd, vl_durationInd);  
    		}

    	}
    }

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

	    int rowInd_plus_one_cd4;
	    int rowInd_plus_one_vl;

	    if(ihiv == 1 & icd4 > 0 & ivl > 0 & iart == 0) { // Applies only to HIV+ individuals not on ART
	    	 // Efflux
	    	// CD4
	    	if(icd4 < 5) { // Can 't leave the last CD4 category
	    		pop(rowInd, diffInd) -= pop(rowInd, countInd) * (1- exp(-time_step/cd4_prog[imale][ivl-1][icd4-1]));
	    	}
	    	// VL
	    	if(ivl < 5) { // Can't leave the last VL category
	    		pop(rowInd, diffInd) -= pop(rowInd, countInd) * (1- exp(-time_step/vl_prog[imale][ivl-1][icd4-1]));
	    	}


	    	// Influx: add this to compartment of one CD4/VL category later (for all but the last age group). This seems easier in an array.
	    	// CD4
	    	rowInd_plus_one_cd4 = rowInd + nVl*nCirc*nCondom*nPrep*nArt;

	    	// std::cout << "rowInd: " << rowInd << std::endl;

	    	if(icd4 < (nCD4 - 1)) {

	    		// std::cout << "cd4_plus_one: " << rowInd_plus_one_vl << std::endl;
	    		pop(rowInd_plus_one_cd4, diffInd) += pop(rowInd, countInd) * (1- exp(-time_step/cd4_prog[imale][ivl-1][icd4-1]));
	    		
	    	}


	    	// VL
	    	rowInd_plus_one_vl = rowInd + nCirc*nCondom*nPrep*nArt;

	    	if(ivl < (nVl - 1)) {

	    		//std::cout << "vl_plus_one: " << rowInd_plus_one_vl << std::endl;

	    		pop(rowInd_plus_one_vl, diffInd) += pop(rowInd, countInd) * (1- exp(-time_step/vl_prog[imale][ivl-1][icd4-1]));
	    		
	    	}
	    }
	   
	   
	}
}

	// std::cout << "head: pop" << pop.block(0,0, 10, 13) << std::endl;



// int main() {
//     clock_t tStart;
//     clock_t tEnd;
//     Eigen::MatrixXd pop = readCSV("agePop.out", pop_cols, pop_rows);
//     tStart = clock();
//     progressDisease(pop);
//     tEnd = clock();
//     std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
//     writeCSV(pop, "progressDisease.cout");
// }

