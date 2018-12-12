#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>
#include "csvUtil.h"
#include "globals.h"

//

int art_cols = 5;
int art_rows = 410;

Eigen::MatrixXd art_cov = readCSV("art_cov.csv", art_cols, art_rows);


void distributeART(int time_index){

    Eigen::VectorXd art_cov_row = art_cov.row(time_index);

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
    }

    // sum over pop
    // order is important!
    // tricky handling the summation
    int hiv = 1; // we only care about hiv compartments
    double artSum;
    for (int age : ageBins){
        for (int male : maleBins){
            for (int risk : riskBins){
                for (int cd4 : cd4Bins){
                    for (int vl : vlBins){
                        for (int circ : circBins){
                            for (int prep : prepBins){
                                for (int condom : condomBins){
                                    artSum = 0;
                                    // compile sum over art for each unique combo of other bins
                                    //make sure you're summing index is always last
                                    // in this case it happens to be by default
                                    for (int art : artBins){
                                        artSum += popCount[hiv][age][male][risk][cd4][vl][circ][prep][condom][art];
                                    }
                                    // now apply the sum
                                    for (int art : artBins){
                                        popCount[hiv][age][male][risk][cd4][vl][circ][prep][condom][art] = artSum * prop[cd4][art];
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




// clang++ -O3 -std=c++11 -g distributeArt.cpp csvUtil.cpp globals.cpp
// int main(){
//     int timeIndex = 0;
//     clock_t tStart;
//     clock_t tEnd;
//     initPop("pop_0.out");
//     tStart = clock();

//     distributeART(timeIndex); //0 based

//     tEnd = clock();
//     std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;

//     std::stringstream filename;
//     filename << "distributeART_" << timeIndex << ".cout";

//     writePop(filename.str(), timeIndex);
//     return 0;
// }
