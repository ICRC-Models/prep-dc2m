#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>
#include "csvUtil.h"
#include "globals.h"

// Read in CD4 and VL disease progression parameters
int dis_prog_cols = 7;
int dis_prog_rows = 50;
Eigen::MatrixXd dis_prog_mat = readCSV("dis_prog.csv", dis_prog_cols, dis_prog_rows); // male, vl, cd4, cd4_duration, vl_duration, hiv, art
const int cd4_durationInd = 3; // Column of dis_prog_mat that contains mean cd4_duration
const int vl_durationInd = 4; // Column of dis_prog_mat that contains mean vl_duration

void progressDisease() {

    // Set up dis_prog arrays - ignore HIV and ART columns. Notice this reorders with respect to R output. This is breaking - indexing is wrong somewhere
    double cd4_prog[nMale][nVl-1][nCD4-1] = {0};
    // initialize outside!!!
    for(int ii : maleBins) {
    	for(int jj : vlBins){
    		for(int kk : cd4Bins) {
    			int rowInd;
    			rowInd = ii * (nVl - 1)*(nCD4 - 1) + jj * (nCD4-1) + kk;
    			cd4_prog[ii][jj][kk] = dis_prog_mat(rowInd, cd4_durationInd);
    		}

    	}
    }

    double vl_prog[nMale][nVl-1][nCD4-1] = {0};

    for(int ii : maleBins) {
    	for(int jj : vlBins){
    		for(int kk : cd4Bins) {
    			int rowInd;
    			rowInd = ii * (nVl - 1)*(nCD4 - 1) + jj * (nCD4-1) + kk;
    			vl_prog[ii][jj][kk] = dis_prog_mat(rowInd, vl_durationInd);
    		}
    	}
    }


    double count;
    int hiv = 1;
    // cd4 > 0 special handling inside loop
    // vl > 0 special handling inside loop
    int art = 0;
    // CD4 Influx - need to get counts from previous CD4 and VL categories, calculate how many people leave the category, increment the category by 1,
    // and merge back to the original data frame and add that number of people.  Probably a cleaner way of doing this
    for (int age : ageBins){
        for (int male : maleBins){
            for (int risk : riskBins){
                for (int cd4 = 1; cd4 < nCD4; cd4++){
                    for (int vl = 1; vl < nVl; vl++){
                        for (int circ : circBins){
                            for (int prep : prepBins){
                                for (int condom : condomBins){
                                    // do stuff
                                    count = popCount[hiv][age][male][risk][cd4][vl][circ][prep][condom][art];
                                    if (cd4 < 5){
                                        popDiff[hiv][age][male][risk][cd4][vl][circ][prep][condom][art] -= count * (1- exp(-time_step/cd4_prog[male][vl-1][cd4-1]));
                                    }
                                    if (vl < 5){
                                        popDiff[hiv][age][male][risk][cd4][vl][circ][prep][condom][art] -= count * (1- exp(-time_step/vl_prog[male][vl-1][cd4-1]));
                                    }
                                    // influx
                                    // CD4
                                    if(cd4 < (nCD4 - 1)) {
                                        // check this allen!!!!
                                        popDiff[hiv][age][male][risk][cd4+1][vl][circ][prep][condom][art] += count * (1- exp(-time_step/cd4_prog[male][vl-1][cd4-1]));
                                    }
                                    // VL
                                    if(vl < (nVl - 1)) {
                                        popDiff[hiv][age][male][risk][cd4][vl+1][circ][prep][condom][art] += count * (1- exp(-time_step/vl_prog[male][vl-1][cd4-1]));
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




// clang++ -O3 -std=c++11 -g progressDisease.cpp csvUtil.cpp globals.cpp
int main(){

    int timeIndex = 0;
    clock_t tStart;
    clock_t tEnd;
    initPop("agePop_0.out");
    tStart = clock();

    progressDisease(); //0 based

    tEnd = clock();
    std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;

    std::stringstream filename;
    filename << "progressDisease_" << timeIndex << ".cout";

    writePop(filename.str(), timeIndex);
    return 0;
}

