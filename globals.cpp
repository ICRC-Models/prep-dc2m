#include <iostream>
#include <iomanip>
#include <fstream>
#include "globals.h"
#include "csvUtil.h"

double popCount[nHIV][nAge][nMale][nRisk][nCD4][nVl][nCirc][nPrep][nCondom][nArt] = {0};
double popDiff[nHIV][nAge][nMale][nRisk][nCD4][nVl][nCirc][nPrep][nCondom][nArt] = {0};

Eigen::MatrixXd risk_props_mat = readCSV("risk_props.csv", risk_cols, risk_rows);

void initPop(std::string filename){
    // populate popCount/Diff table from csv file input
    int countInd = 10;
    int diffInd = 11;
    // read in pop from csv output from R, massage into our new format
    Eigen::MatrixXd csvPop = readCSV(filename, pop_cols, pop_rows);
    // sum over all rows in pop
    double count,diff;
    int ihiv, iage, imale, irisk, icd4, ivl, icirc, iprep, icondom, iart;

    for (int ii=0; ii<pop_rows; ii++){
        count = csvPop(ii,countInd);
        diff = csvPop(ii, diffInd);
        ihiv = csvPop(ii,hivInd);
        iage = csvPop(ii,ageInd) - 1; // 1 indexed fucker
        imale = csvPop(ii,maleInd);
        irisk = csvPop(ii,riskInd) - 1; // 1 indexed fucker
        icd4 = csvPop(ii,cd4Ind);
        ivl = csvPop(ii,vlInd);
        icirc = csvPop(ii,circInd);
        iprep = csvPop(ii,prepInd);
        icondom = csvPop(ii,condomInd);
        iart = csvPop(ii, artInd);
        popCount[ihiv][iage][imale][irisk][icd4][ivl][icirc][iprep][icondom][iart] = count;
        popDiff[ihiv][iage][imale][irisk][icd4][ivl][icirc][iprep][icondom][iart] = diff;
    }
}

void writePop(std::string filename, int timeStep = 0){
    double popVal, diffVal;
    std::ofstream file(filename);
    // order of loop here matches order saved in rscript, for unit testability
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
                                            file << hiv << ",";
                                            file << age + 1 << ","; // undo 0 index
                                            file << male << ",";
                                            file << risk + 1 << ",";  // undo 0 index
                                            file << cd4 << ",";
                                            file << vl << ",";
                                            file << circ << ",";
                                            file << prep << ",";
                                            file << condom << ",";
                                            file << art << ",";
                                            popVal = popCount[hiv][age][male][risk][cd4][vl][circ][prep][condom][art];
                                            diffVal = popDiff[hiv][age][male][risk][cd4][vl][circ][prep][condom][art];
                                            std::stringstream popValStr;
                                            popValStr << std::fixed << std::setprecision(20) << popVal;
                                            std::stringstream diffValStr;
                                            diffValStr << std::fixed << std::setprecision(20) << diffVal;
                                            // \n means new line
                                            file << popValStr.str() << "," << diffValStr.str() << "," << timeStep << "\n";
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


void writeMixMat(int timeStep = 0){
    std::stringstream filename;
    filename << "mixing_matrix_" << timeStep << ".cout";
    std::ofstream mixMatOut (filename.str());
    if(mixMatOut.is_open()){
        for(int ii = 0; ii < nAge; ii++) {
            for(int jj = 0; jj < nMale; jj++) {
                for(int kk = 0; kk < nRisk; kk++) {
                    for(int ii_p = 0; ii_p < nAge; ii_p++) {
                        for(int jj_p = 0; jj_p < nMale; jj_p++) {
                            for(int kk_p = 0; kk_p < nRisk; kk_p++) {
                                if(jj != jj_p) { // Don't print same-sex mixing, since those rows are dropped from the R output
                                     mixMatOut << (ii+1) << "," << jj << "," << (kk+1) << "," << (ii_p + 1) << "," << jj_p << "," << (kk_p + 1) << ",";
                                     mixMatOut << std::fixed << std::setprecision(20) << mixMat[ii][jj][kk][ii_p][jj_p][kk_p] << "\n";
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void writeAdjustedPartnersMat(int timeStep){
    std::stringstream filename;
    filename << "adjusted_partners_" << timeStep << ".cout";
    // Output array to match R format - maybe this can be moved into a separate function? Just for unit testing.
    std::ofstream adjustedPartnersMatOut (filename.str());
    if(adjustedPartnersMatOut.is_open()){
        for(int ii = 0; ii < nAge; ii++) {
            for(int jj = 0; jj < nMale; jj++) {
                for(int kk = 0; kk < nRisk; kk++) {
                    for(int ii_p = 0; ii_p < nAge; ii_p++) {
                        for(int jj_p = 0; jj_p < nMale; jj_p++) {
                            for(int kk_p = 0; kk_p < nRisk; kk_p++) {

                                if(jj != jj_p) { // Don't print same-sex mixing, since those rows are dropped from the R output

                                 adjustedPartnersMatOut << (ii+1) << "," << jj << "," << (kk+1) << "," << (ii_p + 1) << ","  << (kk_p + 1) << ",";
                                 adjustedPartnersMatOut << std::fixed << std::setprecision(20) << adjustedPartnersMat[ii][jj][kk][ii_p][jj_p][kk_p] << "\n";
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}



void writeLambdaMat(int timeStep){
    std::stringstream filename;
    filename << "lambda_mat_" << timeStep << ".cout";
    // Save lambdaMat for comparison to R output
    std::ofstream lambdaMatOut (filename.str());
    if(lambdaMatOut.is_open()){
        for(int ii = 0; ii < nAge; ii++) {
            for(int jj = 0; jj < nMale; jj++) {
                for(int kk = 0; kk < nRisk; kk++) {

                 lambdaMatOut << (ii+1) << "," << jj << "," << (kk+1) << ",";
                 lambdaMatOut << std::fixed << std::setprecision(20) << lambdaMat[ii][jj][kk] << "\n";

                }
            }
        }
    }
}


