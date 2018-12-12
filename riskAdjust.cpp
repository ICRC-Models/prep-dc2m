#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>
#include "csvUtil.h"
#include "globals.h"

// int pop_cols = 13;
// int pop_rows = 82944;

const double risk_prop_ind = 0; // Column of risk_mat that contains risk proportions (zero indexed)

// Eigen::MatrixXd risk_props_mat = readCSV("risk_props.csv", risk_cols, risk_rows);

void riskAdjust(){

    // Note these are already loaded by addBirths. Have to declare them in the function, but should move them all out eventually

    // sum over pop
    // order is important!
    // tricky handling the summation
    // loop over condoms last for summation
    double riskSum, riskProp;

    for (int hiv : hivBins){
        for (int age : ageBins){
            for (int male : maleBins){
                for (int cd4 : cd4Bins){
                    for (int vl : vlBins){
                        for (int circ : circBins){
                            for (int prep : prepBins){
                                for (int condom : condomBins){
                                    for (int art : artBins){
                                        riskSum = 0;
                                        for (int risk : riskBins){
                                            // do stuff
                                            riskSum += popCount[hiv][age][male][risk][cd4][vl][circ][prep][condom][art];
                                        }
                                        // apply the sum
                                        for (int risk : riskBins){
                                            // do stuff
                                            riskProp = risk_props_mat(age * nMale * nRisk + male * nRisk + risk, risk_prop_ind);
                                            popCount[hiv][age][male][risk][cd4][vl][circ][prep][condom][art] = riskSum * riskProp;
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



// clang++ -O3 -std=c++11 -g riskAdjust.cpp csvUtil.cpp globals.cpp
// int main(){

//     int timeIndex = 0;
//     clock_t tStart;
//     clock_t tEnd;
//     initPop("endPop_0.out");
//     tStart = clock();

//     riskAdjust(); //0 based

//     tEnd = clock();
//     std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;

//     std::stringstream filename;
//     filename << "riskAdjust_" << timeIndex << ".cout";

//     writePop(filename.str(), timeIndex);
//     return 0;
// }

