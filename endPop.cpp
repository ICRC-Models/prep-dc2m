#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>
#include "csvUtil.h"
#include "globals.h"

void endPop() {
    double diff;
    for (int hiv : hivBins){
        for (int male : maleBins){
            for (int risk : riskBins){
                for (int cd4 : cd4Bins){
                    for (int vl : vlBins){
                        for (int circ : circBins){
                            for (int prep : prepBins){
                                for (int condom : condomBins){
                                    for (int art : artBins){
                                        for (int age : ageBins){
                                            // Add diff to count and set diff back to zero
                                            diff = popDiff[hiv][age][male][risk][cd4][vl][circ][prep][condom][art];
                                            popCount[hiv][age][male][risk][cd4][vl][circ][prep][condom][art] += diff;
                                            popDiff[hiv][age][male][risk][cd4][vl][circ][prep][condom][art] = 0;

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


// clang++ -O3 -std=c++11 -g endPop.cpp csvUtil.cpp globals.cpp
// int main(){

//     int timeIndex = 0;
//     clock_t tStart;
//     clock_t tEnd;
//     initPop("transmit_0.out");
//     tStart = clock();

//     endPop(); //0 based

//     tEnd = clock();
//     std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;

//     std::stringstream filename;
//     filename << "endPop_" << timeIndex << ".cout";

//     writePop(filename.str(), timeIndex);
//     return 0;
// }
