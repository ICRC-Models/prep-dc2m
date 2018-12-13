#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>
#include "csvUtil.h"
#include "globals.h"


void subtractDeaths(int time_index){
	// For use in the loop
  	double mu, alpha, diff, count;

    for (int hiv : hivBins){
        for (int age : ageBins){
            for (int male : maleBins){
                for (int risk : riskBins){
                    for (int cd4 : cd4Bins){
                        for (int vl : vlBins){
                            for (int circ : circBins){
                                for (int prep : prepBins){
                                    for (int condom : condomBins){
                                        for (int art : artBins){
                                            count = popCount[hiv][age][male][risk][cd4][vl][circ][prep][condom][art];
                                            diff = 0; // Tracker for number of deaths

                                            // Background mortality applies to everyone - find correct multiplier
                                            mu = back_mort[age][male];
                                            diff += count * mu;

                                            // HIV-specific mortality
                                            if(hiv == 1) {
                                                alpha = hiv_mort[age][cd4][art];
                                                diff += count * alpha;
                                            }
                                            // Add to diff column of pop
                                            popDiff[hiv][age][male][risk][cd4][vl][circ][prep][condom][art] -= diff;
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

// clang++ -O3 -std=c++11 -g subtractDeaths.cpp csvUtil.cpp globals.cpp
// int main(){

//     int timeIndex = 0;
//     clock_t tStart;
//     clock_t tEnd;
//     initPop("addBirths_0.out");
//     tStart = clock();

//     subtractDeaths(timeIndex); //0 based

//     tEnd = clock();
//     std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;

//     std::stringstream filename;
//     filename << "subtractDeaths_" << timeIndex << ".cout";

//     writePop(filename.str(), timeIndex);
//     return 0;


// }
