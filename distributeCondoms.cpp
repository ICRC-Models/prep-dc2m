#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>

// clang++ -O3 -std=c++11 distributeCondoms.cpp csvUtil.cpp
//

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

int condom_cols = 12; // Condom coverage is specified for each age group
int condom_rows = 410; // One row for each time step

// int pop_cols = 13;
// int pop_rows = 82944;

Eigen::MatrixXd condom_cov = readCSV("condom_cov.csv", condom_cols, condom_rows);


void distributeCondoms(Eigen::MatrixXd &pop, int time_index){
    // load parameters

    Eigen::VectorXd condom_cov_row = condom_cov.row(time_index);
    // std::cout << "cov row " << condom_cov_row << std::endl;
    // std::cout << condom_cov_row << std::endl;
    const int nHIV = 2;
    const int nAge = 12;
    const int nMale = 2;
    const int nRisk = 3;
    const int nCD4 = 6;
    const int nVl = 6;
    const int nCirc = 2;
    const int nPrep = 2;
    const int nCondom = 2;
    const int nArt = 2; // art is 0 or 1
    const int nPopRows = pop.rows();

    double prop[nAge][nCondom];
    for(int ii=0; ii<nAge; ii++){

            prop[ii][0] = 1 - condom_cov_row(ii);
            prop[ii][1] = condom_cov_row(ii);

        // std::cout << "prop [" << ii << "," << 0 << "]=" << 1 - condom_cov_row(ii-1) << std::endl;
        // std::cout << "prop [" << ii << "," << 1 << "]=" << condom_cov_row(ii-1) << std::endl;
    }



    // int hiv[nHIV] = {0, 1};
    // int age[nAge] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    // int male[nMale] = {0, 1};
    // int risk[nRisk] = {1, 2, 3};
    // int vl[nVl] = {0, 1, 2, 3, 4, 5};
    // int prep[nPrep] = {0, 1};
    // int condom[nCondom] = {0, 1};
    // int art[nArt] = {0, 1};

    // initialize to all zeros - includes all the stratifying variables for summation (everything except for condom)
    double countSum[nHIV][nAge][nMale][nRisk][nCD4][nVl][nCirc][nPrep][nArt] = {0};

    // used in loop
    int ihiv, iage, imale, irisk, icd4, ivl, icirc, iprep, iart;
    double count;

    // sum over all rows in pop
    for (int ii=0; ii<nPopRows; ii++){
        // auto pop_row = pop.row(ii);
        // std::cout << "row " << ii << " of " << nPopRows << std::endl;
        count = pop(ii,countInd);
       //  if (ii==0){
        //     std::cout << "first line count " << count << std::endl;
        // }
        // std:: cout << "count: " << count << std::endl;
        ihiv = pop(ii,hivInd);
        iage = pop(ii,ageInd) - 1; // 1 indexed fucker
        imale = pop(ii,maleInd);
        irisk = pop(ii,riskInd) - 1; // 1 indexed fucker
        icd4 = pop(ii,cd4Ind);
        ivl = pop(ii,vlInd);
        icirc = pop(ii,circInd);
        iprep = pop(ii,prepInd);
        iart = pop(ii,artInd);
        // std::cout << "ihiv: " << ihiv << "\n"
        // << "iage: " << iage << "\n"
        // << "imale: " << imale << "\n"
        // << "irisk: " << irisk << "\n"
        // << "icd4: " << icd4 << "\n"
        // << "ivl: " << ivl << "\n"
        // << "icirc: " << icirc << "\n"
        // << "iprep: " << iprep << "\n"
        // << "icondom: " << icondom << "\n" << std::endl;
        countSum[ihiv][iage][imale][irisk][icd4][ivl][icirc][iprep][iart] += count;
        // std::cout << "sum: " << countSum[ihiv][iage][imale][irisk][icd4][ivl][icirc][iprep][icondom] << std::endl;
    }

    double daSum, daProp;
    int icondom;

    // finally modify pop table
    for (int rowInd=0; rowInd < nPopRows; rowInd++){
        // std:: cout << "hiv inds: " << rowInd << std::endl;
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

        daSum = countSum[ihiv][iage][imale][irisk][icd4][ivl][icirc][iprep][iart];
        daProp = prop[iage][icondom];

        // if(rowInd == 0 | rowInd == 2) {
        //     std::cout << "rowInd: " << rowInd << std::endl;
        //     std::cout << "daSum: " << daSum << std::endl;
        //     std::cout << "daProp: " << daProp << std::endl;
        // }

        pop(rowInd,countInd) = daSum * daProp;
    }



}

// int main(){
//     clock_t tStart;
//     clock_t tEnd;
//     Eigen::MatrixXd pop = readCSV("distributeART.out", pop_cols, pop_rows);
//     tStart = clock();
//     distributeCondoms(pop, 409); //0 based
//     tEnd = clock();
//     std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
//     writeCSV(pop, "distributeCondoms.cout");
// }
