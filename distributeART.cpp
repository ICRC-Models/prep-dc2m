#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>

// clang++ -O3 -std=c++11 distributeArt.cpp csvUtil.cpp
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

int art_cols = 5;
int art_rows = 410;

// int pop_cols = 13;
// int pop_rows = 82944;

Eigen::MatrixXd art_cov = readCSV("art_cov.csv", art_cols, art_rows);


void distributeART(Eigen::MatrixXd &pop, int time_index){
    // load parameters

    Eigen::VectorXd art_cov_row = art_cov.row(time_index);
    // std::cout << "cov row " << art_cov_row << std::endl;
    // std::cout << art_cov_row << std::endl;
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

    double prop[nCD4][nArt];
    for(int ii=0; ii<nCD4; ii++){
        if (ii==0){
            prop[ii][0] = 1;
            prop[ii][1] = 0;
        }
        else {
            prop[ii][0] = 1 - art_cov_row(ii-1);
            prop[ii][1] = art_cov_row(ii-1);
        }
        // std::cout << "prop [" << ii << "," << 0 << "]=" << 1 - art_cov_row(ii-1) << std::endl;
        // std::cout << "prop [" << ii << "," << 1 << "]=" << art_cov_row(ii-1) << std::endl;
    }



    // int hiv[nHIV] = {0, 1};
    // int age[nAge] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    // int male[nMale] = {0, 1};
    // int risk[nRisk] = {1, 2, 3};
    // int vl[nVl] = {0, 1, 2, 3, 4, 5};
    // int prep[nPrep] = {0, 1};
    // int condom[nCondom] = {0, 1};
    // int art[nArt] = {0, 1};

    // find indices in pop where hiv =1;

    auto randomPopRow = pop.row(102);
    std::cout << "random pop " << randomPopRow << std::endl;

    std::vector<int> hivInds1;
    for (int ii = 0; ii<nPopRows; ii++){
        double test = pop(ii, hivInd);
        if (test > 0){
            // std::cout << "test " << test <<std::endl;
        }
        if (pop(ii, hivInd) == 1){
            // std::cout <<"got hiv ind" << ii << std::endl;
            hivInds1.push_back(ii);
        }
    }

    // initialize to all zeros
    // count is integer right?
    double countSum[nHIV][nAge][nMale][nRisk][nCD4][nVl][nCirc][nPrep][nCondom] = {0};

    // used in loop
    int ihiv, iage, imale, irisk, icd4, ivl, icirc, iprep, icondom;
    double count;

    // sum over all rows in pop
    for (int ii=0; ii<nPopRows; ii++){
        // auto pop_row = pop.row(ii);
        // std::cout << "row " << ii << " of " << nPopRows << std::endl;
        count = pop(ii,countInd);
        if (ii==0){
            std::cout << "first line count " << count << std::endl;
        }
        // std:: cout << "count: " << count << std::endl;
        ihiv = pop(ii,hivInd);
        iage = pop(ii,ageInd) - 1; // 1 indexed fucker
        imale = pop(ii,maleInd);
        irisk = pop(ii,riskInd) - 1; // 1 indexed fucker
        icd4 = pop(ii,cd4Ind);
        ivl = pop(ii,vlInd);
        icirc = pop(ii,circInd);
        iprep = pop(ii,prepInd);
        icondom = pop(ii,condomInd);
        // std::cout << "ihiv: " << ihiv << "\n"
        // << "iage: " << iage << "\n"
        // << "imale: " << imale << "\n"
        // << "irisk: " << irisk << "\n"
        // << "icd4: " << icd4 << "\n"
        // << "ivl: " << ivl << "\n"
        // << "icirc: " << icirc << "\n"
        // << "iprep: " << iprep << "\n"
        // << "icondom: " << icondom << "\n" << std::endl;
        countSum[ihiv][iage][imale][irisk][icd4][ivl][icirc][iprep][icondom] += count;
        // std::cout << "sum: " << countSum[ihiv][iage][imale][irisk][icd4][ivl][icirc][iprep][icondom] << std::endl;
    }

    double daSum, daProp;
    int iart;

    // finally modify pop table, only rows with hiv == 1
    for (int rowInd : hivInds1){
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

        daSum = countSum[ihiv][iage][imale][irisk][icd4][ivl][icirc][iprep][icondom];
        daProp = prop[icd4][iart];

        if(rowInd-1 == 55985 | rowInd-1 == 55986) {

        	std::cout <<  "icd4: " << icd4 << std::endl;
        	std::cout <<  "iart: " << iart << std::endl;
        	std::cout << "daProp: " << daProp << std::endl;
        }


        pop(rowInd,countInd) = daSum * daProp;
    }



}

// int main(){
//     clock_t tStart;
//     clock_t tEnd;
//     Eigen::MatrixXd pop = readCSV("incrt.out", pop_cols, pop_rows);
//     tStart = clock();

//     distributeART(pop, 409); //0 based

//     tEnd = clock();
//     std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
//     writeCSV(pop, "distributeART.cout");
// }
