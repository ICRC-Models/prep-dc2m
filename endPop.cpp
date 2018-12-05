#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>

// clang++ -O3 -std=c++11 endPop.cpp csvUtil.cpp

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

void endPop(Eigen::MatrixXd	&pop) {

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

    // Add diff to count and set diff back to zero
    for(int rowInd = 0; rowInd < nPopRows; rowInd++) {

    	pop(rowInd, countInd) += pop(rowInd, diffInd);

    	pop(rowInd, diffInd) = 0;


    }



}


// int main(){
//     clock_t tStart;
//     clock_t tEnd;
//     Eigen::MatrixXd pop = readCSV("transmit.out", pop_cols, pop_rows);
//     tStart = clock();

//     endPop(pop); //0 based

//     tEnd = clock();
//     std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
//     writeCSV(pop, "endPop.cout");
// }
