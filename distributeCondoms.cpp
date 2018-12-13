#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>
#include "csvUtil.h"
#include "globals.h"

int condom_cols = 12; // Condom coverage is specified for each age group
int condom_rows = 410; // One row for each time step


Eigen::MatrixXd condom_cov = readCSV("condom_cov.csv", condom_cols, condom_rows);
double propCond[nAge][nCondom];

void distributeCondoms(int time_index){
    // load parameters

    Eigen::VectorXd condom_cov_row = condom_cov.row(time_index);
    // std::cout << "cov row " << condom_cov_row << std::endl;
    // std::cout << condom_cov_row << std::endl;


    for(int ii=0; ii<nAge; ii++){
        propCond[ii][0] = 1 - condom_cov_row(ii);
        propCond[ii][1] = condom_cov_row(ii);
    }


    // sum over pop
    // order is important!
    // tricky handling the summation
    // loop over condoms last for summation
    double condomSum;
    for (int hiv : hivBins){
        for (int age : ageBins){
            for (int male : maleBins){
                for (int risk : riskBins){
                    for (int cd4 : cd4Bins){
                        for (int vl : vlBins){
                            for (int circ : circBins){
                                for (int prep : prepBins){
                                    for (int art : artBins){
                                        condomSum = 0;
                                        // innermost loop is summation
                                        for (int condom : condomBins){
                                            condomSum += popCount[hiv][age][male][risk][cd4][vl][circ][prep][condom][art];
                                        }
                                        // now apply the sum
                                        for (int condom : condomBins){
                                            popCount[hiv][age][male][risk][cd4][vl][circ][prep][condom][art] = condomSum * propCond[age][condom];
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// int main(){
//     // clang++ -O3 -std=c++11 -g distributeCondoms.cpp csvUtil.cpp globals.cpp
//     int timeIndex = 0;
//     clock_t tStart;
//     clock_t tEnd;
//     initPop("distributeART_0.out");
//     tStart = clock();

//     distributeCondoms(timeIndex); //0 based

//     tEnd = clock();
//     std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;

//     std::stringstream filename;
//     filename << "distributeCondoms_" << timeIndex << ".cout";

//     writePop(filename.str(), timeIndex);
//     return 0;
// }
