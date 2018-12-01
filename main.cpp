#include <iostream>
#include <time.h>
#include <eigen3/Eigen/Dense>

// prototype -- move to header eventually
Eigen::MatrixXd readCSV(std::string filename, int cols, int rows);
void writeCSV(Eigen::MatrixXd matrix, std::string filename);
void distributeART(Eigen::MatrixXd &pop, int time_index);
void distributeCondoms(Eigen::MatrixXd &pop, int time_index);
void addBirths(Eigen::MatrixXd &pop, int time_index);

int main(){

    int timeStep = 409; //0 based
    int pop_cols = 13;
    int pop_rows = 82944;
    Eigen::MatrixXd pop;

    clock_t tStart;
    clock_t tEnd;

    // distributeART
    pop = readCSV("incrt.out", pop_cols, pop_rows);
    tStart = clock();
    distributeART(pop, timeStep); //0 based
    tEnd = clock();
    std::cout << "distributeART time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
    writeCSV(pop, "distributeART.cout");


    // distributeCondoms
    pop = readCSV("distributeART.out", pop_cols, pop_rows);
    tStart = clock();
    distributeCondoms(pop, timeStep); //0 based
    tEnd = clock();
    std::cout << "distributeCondoms time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
    writeCSV(pop, "distributeCondoms.cout");

    // addBirths
    pop = readCSV("distributeCondoms.out", pop_cols, pop_rows);
    tStart = clock();
    addBirths(pop, timeStep); //0 based
    tEnd = clock();
    std::cout << "addBirths time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
    writeCSV(pop, "addBirths.cout");
}
