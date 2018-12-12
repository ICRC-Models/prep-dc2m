#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>
#include "csvUtil.h"
#include "globals.h"


// back_mort header: age,male, mu
int back_mort_cols = 3;
int back_mort_rows = 24;
Eigen::MatrixXd back_mort_mat = readCSV("back_mort.csv", back_mort_cols, back_mort_rows);
const int muInd = 2; // Column of back_mort_mat that contains background mortality coefficient (zero indexed)

// hiv_mort header: age, cd4, alpha, art, hiv
int hiv_mort_cols = 5;
int hiv_mort_rows = 144;
Eigen::MatrixXd hiv_mort_mat = readCSV("hiv_mort.csv", hiv_mort_cols, hiv_mort_rows);
const int alphaInd = 2; // Column of hiv_mort_mat that contains HIV mortality coefficient (zero indexed)



void subtractDeaths(int time_index){

    // Set up back_mort array.
    // again move this guy outside!!!!!!!!
    double back_mort[nAge][nMale] = {0};

    for(int ii : ageBins){
    	for(int jj : maleBins){

    		int rowInd;
    		rowInd = ii * nMale + jj;
            // This line requires back_mort_mat to be ordered by age, cd4, and ART status.
    		back_mort[ii][jj] = back_mort_mat(rowInd, muInd);

    	}
    }

    // Set up hiv_mort array. Drop HIV column - this only applies to HIV+.
    // Note that this reorders with respect to R output.
    // move this guy outside!!!!!
    double hiv_mort[nAge][nCD4][nArt] = {0};

    for(int ii : ageBins){
    	for(int jj : cd4Bins){
    		for(int kk : artBins) {
    			int rowInd;
				rowInd =  ii * nCD4 * nArt + jj * nArt + kk;
                // This line requires hiv_mort_mat to be ordered by age, cd4, and ART status.
	    		hiv_mort[ii][jj][kk] = hiv_mort_mat(rowInd, alphaInd);
    		}
    	}
    }

	// Subtract deaths from pop
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
