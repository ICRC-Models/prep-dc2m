#include <iostream>
#include <iomanip>
#include <fstream>
#include "globals.h"
#include "csvUtil.h"

double popCount[nHIV][nAge][nMale][nRisk][nCD4][nVl][nCirc][nPrep][nCondom][nArt] = {0};
double popDiff[nHIV][nAge][nMale][nRisk][nCD4][nVl][nCirc][nPrep][nCondom][nArt] = {0};

void initPop(std::string filename){
    int countInd = 10;
    // read in pop from csv output from R, massage into our new format
    Eigen::MatrixXd csvPop = readCSV(filename, pop_cols, pop_rows);
    // sum over all rows in pop
    double count;
    int ihiv, iage, imale, irisk, icd4, ivl, icirc, iprep, icondom, iart;

    for (int ii=0; ii<pop_rows; ii++){
        count = csvPop(ii,countInd);
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
    }
}

void writePop(std::string filename, int timeStep = 0){
    // do we need to hack in a diff column here? i think so
    // but it seems diff only appears after loop 0 which makes
    // some sense i guess?
    bool printFirst = true;
    double popVal;
    std::ofstream file(filename);
    // order of loop here matches order saved in rscript
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
                                            std::stringstream popValStr;
                                            popValStr << std::fixed << std::setprecision(20) << popVal;
                                            // \n means new line
                                            file << popValStr.str() << "," << timeStep << "\n";
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



