#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>
#include "csvUtil.h"
#include "globals.h"

// fert header: age,male,gamma,cd4,art
int fert_cols = 5;
int fert_rows = 144;
Eigen::MatrixXd fert_mat = readCSV("fert.csv", fert_cols, fert_rows); // age, male, gamma, cd4, art
const int gammaInd = 2; // Column of fert_mat that contains fertility coefficient (zero indexed)

// Vertical transmission parameters - these are time-varying
int vert_cols = 1;
int vert_rows = 410;
Eigen::MatrixXd vert_trans_mat = readCSV("vert_trans.csv", vert_cols, vert_rows);

// Risk proportions: needed for distributing births by risk status.
// Only actually need the first 12 rows but not sure how to read only those ones in.
int risk_cols = 4;
int risk_rows = 72;
Eigen::MatrixXd risk_props_mat = readCSV("risk_props.csv", risk_cols, risk_rows);
const double risk_prop_ind = 0; // Column of risk_mat that contains risk proportions (zero indexed)


void addBirths(int time_index){

    // Vertical transmission paramter
    const double vert_trans = vert_trans_mat(time_index);
    // std::cout << "vert_trans: " << vert_trans << std::endl;

    // Set up fert array - ignore male column. Notice this reorders with respect to R output
    double fert[nAge][nCD4][nArt] = {0};
    int rowInd;

    for(int ii = 0; ii < nAge; ii++) {
    	for(int jj = 0; jj < nCD4; jj++){
    		for(int kk = 0; kk < nArt; kk++) {
    			rowInd = ii * (nCD4*nArt) + jj * nArt + kk;
                // This line requires fert_mat to be ordered by age, cd4, and ART status.
    			fert[ii][jj][kk] = fert_mat(rowInd, gammaInd);
    		}

    	}
    }

	// Calculate total birhts from HIV- and HIV+ mothers by multiplying gamma
    // in fert by count. Should only operate on women

    double births_from_neg = 0;
    double births_from_pos = 0;
    double gamma;
    double count;

    int male = 0; // only care about women
    for (int hiv : hivBins){
        for (int age : ageBins){
            for (int risk : riskBins){
                for (int cd4 : cd4Bins){
                    for (int vl : vlBins){
                        for (int circ : circBins){
                            for (int prep : prepBins){
                                for (int condom : condomBins){
                                    for (int art : artBins){
                                        gamma = fert[age][cd4][art];
                                        count = popCount[hiv][age][male][risk][cd4][vl][circ][prep][condom][art];
                                        if (hiv==0 || art ==1){
                                            births_from_neg += gamma*count;
                                        }
                                        else {
                                            births_from_pos += gamma*count;
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

    double neg_births = births_from_pos * (1 - vert_trans) + births_from_neg;
    double pos_births = births_from_pos * vert_trans;

    // Distribute births into pop table
    // For use in the loop
    double riskProp, diff, mult;

    // Births are only allocated into these compartments.
    int prep = 0;
    int condom = 0;
    int art = 0;
    int age = 0; // baby (should this be 0, the first compartment?)

    for (int hiv : hivBins){
        for (int male : maleBins){
            for (int risk : riskBins){
                for (int cd4 : cd4Bins){
                    for (int vl : vlBins){
                        for (int circ : circBins){
                            diff = 0;
                            mult = 1;
                            // Find proportion of births going into this risk group
                            riskProp = risk_props_mat(male * nRisk + risk, risk_prop_ind);
                            mult = mult * riskProp;

                            // Allocate across sex and circumcision status
                            if (male == 1) {
                                mult = mult * propMale;

                                if (circ == 1) {
                                    mult = mult * nncirc_prop;
                                }
                                else {
                                    mult = mult * (1-nncirc_prop);
                                }
                            }
                            else {
                                mult = mult * (1-propMale);
                                if(circ == 1) { // No females circumcised
                                    mult = 0;
                                }
                            }

                            // Allocate across HIV status
                            if (hiv == 0) {
                                if (vl == 0 && cd4 == 0 && art == 0) {
                                    diff = neg_births * mult;
                                }
                            }
                            else {
                                if (vl == 1 && cd4 == 1 && art == 0) {
                                    diff = pos_births * mult;
                                }
                            }
                            // Add to diff column of pop
                            popDiff[hiv][age][male][risk][cd4][vl][circ][prep][condom][art] += diff;
                        }
                    }
                }
            }
        }
    }

}


// clang++ -O3 -std=c++11 -g addBirths.cpp csvUtil.cpp globals.cpp
int main(){

    int timeIndex = 0;
    clock_t tStart;
    clock_t tEnd;
    initPop("distributeCondoms_0.out");
    tStart = clock();

    addBirths(timeIndex); //0 based

    tEnd = clock();
    std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;

    std::stringstream filename;
    filename << "addBirths_" << timeIndex << ".cout";

    writePop(filename.str(), timeIndex);
    return 0;


}
