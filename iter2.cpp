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
void endPop(Eigen::MatrixXd &pop);
void riskAdjust(Eigen::MatrixXd &pop);

int main(){

    // int timeIndex = 0; //0 based
    int pop_cols = 13;
    int pop_rows = 82944;
    Eigen::MatrixXd pop;

    clock_t tStart;
    clock_t tEnd;

    char buffer[50];

    int steps = 410;

    for(int timeIndex = 0; timeIndex < steps; timeIndex ++) {

        std::cout << "timeIndex: " << timeIndex << std::endl;

        // // distributeART
        // sprintf(buffer, "pop_%i.out", timeIndex);
        // pop = readCSV(buffer, pop_cols, pop_rows);
        // tStart = clock();
        // distributeART(pop, timeIndex); //0 based
        // tEnd = clock();
        // std::cout << "distributeART time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
        // sprintf(buffer, "distributeART_%i.cout", timeIndex);
        // writeCSV(pop, buffer);
    
        // // distributeCondoms
        // sprintf(buffer, "distributeART_%i.out", timeIndex);
        // pop = readCSV(buffer, pop_cols, pop_rows);
        // tStart = clock();
        // distributeCondoms(pop, timeIndex); //0 based
        // tEnd = clock();
        // std::cout << "distributeCondoms time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
        // sprintf(buffer, "distributeCondoms_%i.cout", timeIndex);
        // writeCSV(pop, buffer);
    
        // // addBirths
        // sprintf(buffer, "distributeCondoms_%i.out", timeIndex);
        // pop = readCSV(buffer, pop_cols, pop_rows);
        // tStart = clock();
        // addBirths(pop, timeIndex); //0 based
        // tEnd = clock();
        // std::cout << "addBirths time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
        // sprintf(buffer, "addBirths_%i.cout", timeIndex);
        // writeCSV(pop, buffer);
    
        // //subtractDeaths
        // sprintf(buffer, "addBirths_%i.out", timeIndex);
        // pop = readCSV(buffer, pop_cols, pop_rows);
        // tStart = clock();
        // subtractDeaths(pop, timeIndex); //0 based
        // tEnd = clock();
        // std::cout << "subtractDeaths time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
        // sprintf(buffer, "subtractDeaths_%i.cout", timeIndex);
        // writeCSV(pop, buffer);
    
        // //agePop
        // sprintf(buffer, "subtractDeaths_%i.out", timeIndex);
        // pop = readCSV(buffer, pop_cols, pop_rows);
        // tStart = clock();
        // agePop(pop);
        // tEnd = clock();
        // std::cout << "agePop time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
        // sprintf(buffer, "agePop_%i.cout", timeIndex);
        // writeCSV(pop, buffer);
    
        // //progressDisease
        // sprintf(buffer, "agePop_%i.out", timeIndex);
        // pop = readCSV(buffer, pop_cols, pop_rows);
        // tStart = clock();
        // progressDisease(pop);
        // tEnd = clock();
        // std::cout << "progressDisease time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
        // sprintf(buffer, "progressDisease_%i.cout", timeIndex);
        // writeCSV(pop, buffer);
    
        //Transmit
        sprintf(buffer, "progressDisease_%i.out", timeIndex);
        pop = readCSV(buffer, pop_cols, pop_rows);
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
        sprintf(buffer, "transmit_%i.cout", timeIndex);
        writeCSV(pop, buffer);
    
        sprintf(buffer, "transmit_%i.out", timeIndex);
        pop = readCSV(buffer, pop_cols, pop_rows);
        tStart = clock();
        endPop(pop);
        tEnd = clock();
        std::cout << "endPop took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
        sprintf(buffer, "endPop_%i.cout", timeIndex);
        writeCSV(pop, buffer);
    
        // sprintf(buffer, "endPop_%i.out", timeIndex);
        // pop = readCSV(buffer, pop_cols, pop_rows);
        // tStart = clock();
        // riskAdjust(pop);
        // tEnd = clock();
        // std::cout << "riskAdjust took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
        // sprintf(buffer, "riskAdjust_%i.cout", timeIndex);
        // writeCSV(pop, buffer);
    }
}
