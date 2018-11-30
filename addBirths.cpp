#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>

// clang++ -O3 -std=c++11 addBirths.cpp csvUtil.cpp

// prototype -- move to header eventually
Eigen::MatrixXd readCSV(std::string filename, int cols, int rows);
void writeCSV(Eigen::MatrixXd matrix, std::string filename);

int pop_cols = 13;
int pop_rows = 82944;

double nncirc_prop = 0.1; // Neonatal circumcision prevalence

// fert header: age,male,gamma,cd4,art
int fert_cols = 5;
int fert_rows = 144;
Eigen::MatrixXd fert = readCSV("fert.csv", fert_cols, fert_rows);



void addBirths(Eigen::MatrixXd &pop){


}

int main(){
    clock_t tStart;
    clock_t tEnd;
    Eigen::MatrixXd pop = readCSV("distributeCondoms.out", pop_cols, pop_rows);
    tStart = clock();
    addBirths(pop);
    tEnd = clock();
    std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
    writeCSV(pop, "addBirths.cout");
}