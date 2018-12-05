#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>

// clang++ -O3 -std=c++11 riskAdjust.cpp csvUtil.cpp

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

// prototype -- move to header eventually
Eigen::MatrixXd readCSV(std::string filename, int cols, int rows);
void writeCSV(Eigen::MatrixXd matrix, std::string filename);

// int pop_cols = 13;
// int pop_rows = 82944;

void riskAdjust(Eigen::MatrixXd &pop){

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

    // Note these are already loaded by addBirths. Have to declare them in the function, but should move them all out eventually
	int risk_cols = 4;
	int risk_rows = 72;
	Eigen::MatrixXd risk_props_mat = readCSV("risk_props.csv", risk_cols, risk_rows);
	const double risk_prop_ind = 0; // Column of risk_mat that contains risk proportions (zero indexed)


    // Sum population across risk categories for each compartment
    double sumsMat[nHIV][nAge][nMale][nCD4][nVl][nCirc][nPrep][nCondom][nArt] = {0};
    
    int ihiv, iage, imale, irisk, icd4, ivl, icirc, iprep, icondom, iart;

    for(int rowInd = 0; rowInd < nPopRows; rowInd++) {

    	ihiv = pop(rowInd,hivInd);
        iage = pop(rowInd,ageInd) - 1; // 1 indexed fucker
        imale = pop(rowInd,maleInd);
        // irisk = pop(rowInd,riskInd) - 1; // 1 indexed fucker
        icd4 = pop(rowInd,cd4Ind);
        ivl = pop(rowInd,vlInd);
        icirc = pop(rowInd,circInd);
        iprep = pop(rowInd,prepInd);
        icondom = pop(rowInd,condomInd);
        iart = pop(rowInd, artInd);

    	sumsMat[ihiv][iage][imale][icd4][ivl][icirc][iprep][icondom][iart] += pop(rowInd, countInd);

    }

    // Multiply sums by risk proportions defined initially. I don't really like this approach...
     double riskProp;
     for(int rowInd = 0; rowInd < nPopRows; rowInd++) {

    	ihiv = pop(rowInd,hivInd);
        iage = pop(rowInd,ageInd) - 1; // 1 indexed fucker
        imale = pop(rowInd,maleInd);
        irisk = pop(rowInd,riskInd) - 1; // 1 indexed fucker
        icd4 = pop(rowInd,cd4Ind);
        ivl = pop(rowInd,vlInd);
        icirc = pop(rowInd,circInd);
        iprep = pop(rowInd,prepInd);
        icondom = pop(rowInd,condomInd);
        iart = pop(rowInd, artInd);

        // Find risk proportions indicator
        riskProp = risk_props_mat(iage * nMale * nRisk + imale * nRisk + irisk, risk_prop_ind);

    	pop(rowInd, countInd) = riskProp * sumsMat[ihiv][iage][imale][icd4][ivl][icirc][iprep][icondom][iart];

    }



}

// int main(){
//     clock_t tStart;
//     clock_t tEnd;
//     Eigen::MatrixXd pop = readCSV("endPop.out", pop_cols, pop_rows);
//     tStart = clock();
//     riskAdjust(pop); 
//     tEnd = clock();
//     std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
//     writeCSV(pop, "riskAdjust.cout");
// }

