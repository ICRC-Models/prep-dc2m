#include <iostream>
#include <time.h>
#include <eigen3/Eigen/Dense>

// prototype -- move to header eventually
Eigen::MatrixXd readCSV(std::string filename, int cols, int rows);
void writeCSV(Eigen::MatrixXd matrix, std::string filename);
void distributeART(Eigen::MatrixXd &pop, int time_index);
void distributeCondoms(Eigen::MatrixXd &pop, int time_index);
void addBirths(Eigen::MatrixXd &pop, int time_index);
void subtractDeaths(Eigen::MatrixXd &pop, int time_index);
void agePop(Eigen::MatrixXd &pop);
void progressDisease(Eigen::MatrixXd &pop);
void calcMixMat(Eigen::MatrixXd &pop, int time_index);
void adjustPartnerships();
void calcLambda(Eigen::MatrixXd &pop);
void transmit(Eigen::MatrixXd &pop);

int main(){

    int timeIndex = 409; //0 based
    int pop_cols = 13;
    int pop_rows = 82944;
    Eigen::MatrixXd pop;

    clock_t tStart;
    clock_t tEnd;

    // distributeART
    pop = readCSV("incrt.out", pop_cols, pop_rows);
    tStart = clock();
    distributeART(pop, timeIndex); //0 based
    tEnd = clock();
    std::cout << "distributeART time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
    writeCSV(pop, "distributeART.cout");

    // distributeCondoms
    pop = readCSV("distributeART.out", pop_cols, pop_rows);
    tStart = clock();
    distributeCondoms(pop, timeIndex); //0 based
    tEnd = clock();
    std::cout << "distributeCondoms time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
    writeCSV(pop, "distributeCondoms.cout");

    // addBirths
    pop = readCSV("distributeCondoms.out", pop_cols, pop_rows);
    tStart = clock();
    addBirths(pop, timeIndex); //0 based
    tEnd = clock();
    std::cout << "addBirths time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
    writeCSV(pop, "addBirths.cout");

    //subtractDeaths
    pop = readCSV("addBirths.out", pop_cols, pop_rows);
    tStart = clock();
    subtractDeaths(pop, timeIndex); //0 based
    tEnd = clock();
    std::cout << "subtractDeaths time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
    writeCSV(pop, "subtractDeaths.cout");

    //agePop
    pop = readCSV("subtractDeaths.out", pop_cols, pop_rows);
    tStart = clock();
    agePop(pop);
    tEnd = clock();
    std::cout << "agePop time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
    writeCSV(pop, "agePop.cout");

    //progressDisease
    pop = readCSV("agePop.out", pop_cols, pop_rows);
    tStart = clock();
    progressDisease(pop);
    tEnd = clock();
    std::cout << "progressDisease time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
    writeCSV(pop, "progressDisease.cout");

    //Transmit
    pop = readCSV("progressDisease.out", pop_cols, pop_rows);
    tStart = clock();
    calcMixMat(pop, timeIndex);
    tEnd = clock();
    std::cout << "calcMixMat took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;

    tStart = clock();
    adjustPartnerships();
    tEnd = clock();
    std::cout << "adjustPartnerships took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;

    tStart = clock();
    calcLambda(pop);
    tEnd = clock();
    std::cout << "calcLambda took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;

    tStart = clock();
    transmit(pop);
    tEnd = clock();
    std::cout << "transmit took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
    writeCSV(pop, "transmit.cout");


}
