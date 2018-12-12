#include <iostream>
#include <vector>
#include <time.h>
#include "csvUtil.h"
#include "globals.h"

void agePop() {

    double diff, count;

    // loop over age last to
    // get motion between compartments right?
    // not efficient to loop this way
    // but more readable
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
                                            // influx add from previous count for all age bins but first
                                            // do this before we reset count for the current bin
                                            // might be better to put loop in the right order and
                                            // use index + 1 like i did in progress disease
                                            if (age != 0){
                                                popDiff[hiv][age][male][risk][cd4][vl][circ][prep][condom][art] += 0.2 * time_step * count;
                                            }
                                            count = popCount[hiv][age][male][risk][cd4][vl][circ][prep][condom][art];
                                            // Efflux: aging out of the compartment. 1/5 of 5-year age groups leave each compartment
                                            popDiff[hiv][age][male][risk][cd4][vl][circ][prep][condom][art] -= 0.2 * time_step * count;
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



// clang++ -O3 -std=c++11 -g agePop.cpp csvUtil.cpp globals.cpp
int main(){

    int timeIndex = 0;
    clock_t tStart;
    clock_t tEnd;
    initPop("subtractDeaths_0.out");
    tStart = clock();

    agePop(); //0 based

    tEnd = clock();
    std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;

    std::stringstream filename;
    filename << "agePop_" << timeIndex << ".cout";

    writePop(filename.str(), timeIndex);
    return 0;
}
