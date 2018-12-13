#include <iostream>
#include <iomanip>
#include <fstream>
#include <eigen3/Eigen/Dense>
#include "globals.h"
#include "csvUtil.h"

double popCount[nHIV][nAge][nMale][nRisk][nCD4][nVl][nCirc][nPrep][nCondom][nArt] = {0};
double popDiff[nHIV][nAge][nMale][nRisk][nCD4][nVl][nCirc][nPrep][nCondom][nArt] = {0};

// params to be populated

double fert[nAge][nCD4][nArt] = {0};
double back_mort[nAge][nMale] = {0};
double hiv_mort[nAge][nCD4][nArt] = {0};
double cd4_prog[nMale][nVl-1][nCD4-1] = {0};
double vl_prog[nMale][nVl-1][nCD4-1] = {0};
double assortMatRisk[nRisk][nRisk] = {0};
double betas[nMale][nRisk][nVl][nArt] = {0};


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
    filename << "cout/mixing_matrix_" << timeStep << ".cout";
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
    filename << "cout/adjusted_partners_" << timeStep << ".cout";
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
    filename << "cout/lambda_mat_" << timeStep << ".cout";
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

void initParams(){

    // broken out from addBirths
    // fert header: age,male,gamma,cd4,art
    int fert_cols = 5;
    int fert_rows = 144;
    Eigen::MatrixXd fert_mat = readCSV("fert.csv", fert_cols, fert_rows); // age, male, gamma, cd4, art
    int gammaInd = 2; // Column of fert_mat that contains fertility coefficient (zero indexed)
    int rowInd;
    for (int ii : ageBins){
        for(int jj : cd4Bins){
            for(int kk : artBins){
                rowInd = ii * (nCD4*nArt) + jj * nArt + kk;
                // This line requires fert_mat to be ordered by age, cd4, and ART status.
                fert[ii][jj][kk] = fert_mat(rowInd, gammaInd);
            }

        }
    }

    // subtract deaths
    int back_mort_cols = 3;
    int back_mort_rows = 24;
    Eigen::MatrixXd back_mort_mat = readCSV("back_mort.csv", back_mort_cols, back_mort_rows);
    const int muInd = 2; // Column of back_mort_mat that contains background mortality coefficient (zero indexed)
    for(int ii : ageBins){
        for(int jj : maleBins){
            int rowInd;
            rowInd = ii * nMale + jj;
            // This line requires back_mort_mat to be ordered by age, cd4, and ART status.
            back_mort[ii][jj] = back_mort_mat(rowInd, muInd);

        }
    }

// hiv_mort header: age, cd4, alpha, art, hiv
    int hiv_mort_cols = 5;
    int hiv_mort_rows = 144;
    Eigen::MatrixXd hiv_mort_mat = readCSV("hiv_mort.csv", hiv_mort_cols, hiv_mort_rows);
    const int alphaInd = 2; // Column of hiv_mort_mat that contains HIV mortality coefficient (zero indexed)

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

    // progress disease
    // Read in CD4 and VL disease progression parameters
    int dis_prog_cols = 7;
    int dis_prog_rows = 50;
    Eigen::MatrixXd dis_prog_mat = readCSV("dis_prog.csv", dis_prog_cols, dis_prog_rows); // male, vl, cd4, cd4_duration, vl_duration, hiv, art
    const int cd4_durationInd = 3; // Column of dis_prog_mat that contains mean cd4_duration
    const int vl_durationInd = 4; // Column of dis_prog_mat that contains mean vl_duration

    for(int ii : maleBins) {
        for(int jj = 0; jj < nVl-1; jj++){ // note -1
            for(int kk = 0; kk < nCD4-1; kk++) { // note -1
                int rowInd;
                rowInd = ii * (nVl - 1)*(nCD4 - 1) + jj * (nCD4-1) + kk;
                cd4_prog[ii][jj][kk] = dis_prog_mat(rowInd, cd4_durationInd);
            }

        }
    }

    for(int ii : maleBins) {
        for(int jj = 0; jj < nVl-1; jj++){ // note -1
            for(int kk = 0; kk < nCD4-1; kk++) { // note -1
                int rowInd;
                rowInd = ii * (nVl - 1)*(nCD4 - 1) + jj * (nCD4-1) + kk;
                vl_prog[ii][jj][kk] = dis_prog_mat(rowInd, vl_durationInd);
            }
        }
    }

    // transmit
    // Assortative mixing by risk - this is the identity matrix.
    for(int ii = 0; ii < nRisk; ii++) {
        for(int jj = 0; jj < nRisk; jj++) {

            if(ii == jj) {
                assortMatRisk[ii][jj] = 1;
            } else {
                assortMatRisk[ii][jj] = 0;
            }

        }
    }

    int betas_cols = 7;
    int betas_rows = 72;
    Eigen::MatrixXd betas_mat = readCSV("betas.csv", betas_cols, betas_rows);
    int transmissionRiskInd = 6; // Column of betas_mat containing transmission risk

    for(int rowInd = 0; rowInd < betas_rows; rowInd++) {

        // Identify what the columns in betas_mat correspond to
        int imale = betas_mat(rowInd, 0);
        int irisk = betas_mat(rowInd, 1) - 1; // 1 indexed
        int ivl_p = betas_mat(rowInd, 2);
        int iart_p = betas_mat(rowInd, 3);

        betas[imale][irisk][ivl_p][iart_p] = betas_mat(rowInd, transmissionRiskInd);

    }

}
