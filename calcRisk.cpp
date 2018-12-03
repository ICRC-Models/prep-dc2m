#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>

// clang++ -O3 -std=c++11 calcRisk.cpp csvUtil.cpp

// prototype -- move to header eventually
Eigen::MatrixXd readCSV(std::string filename, int cols, int rows);
void writeCSV(Eigen::MatrixXd matrix, std::string filename);

int pop_cols = 13;
int pop_rows = 82944;

// Constants for pop frame. Maybe we should move these outside later?
const int nHIV = 2;
const int nAge = 12;
const int nMale = 2;
const int nRisk = 3;
const int nCD4 = 6; // len of art_cov_row
const int nVl = 6;
const int nCirc = 2;
const int nPrep = 2;
const int nCondom = 2;
const int nArt = 2; // art is 0 or 1

// Input parameters: move these out later
const double time_step = 0.1;
const double theta = 0.5; // parameter that governs the extent to which differences in reported number of sexual partners between males and females is male (1) or female (0) driven

// Mixing matrix: set up as 6D array
double mixMat[nAge][nMale][nRisk][nAge][nMale][nRisk] = {0};

// Partnership inputs
int partners_cols = 4;
int partners_rows = 72;
Eigen::MatrixXd partners = readCSV("partners_mat.csv", partners_cols, partners_rows); // age, male, risk, partners

// Deltas: these parameters govern the mixing pattern for "completely assortative" mixing, which isn't truly "completely assortative" but is allowed to vary. The delta that gets loaded here is the proportion of partnerships that are with the same age group. 1-delta is the proportion that are with the age group one above (females) or below (males).
int deltas_cols = 1;
int deltas_rows = 410; // This is time-step dependent
Eigen::MatrixXd deltas = readCSV("deltas_mat.csv", deltas_cols, deltas_rows);
// double deltaMat_risk[nAge][nMale][nRisk][nAge][nMale][nRisk] = 0;
// double deltaMat_age[nAge][nMale][nRisk][nAge][nMale][nRisk] = 0;

// Mixing matrix for completely assortative mixing
double assortMat[nAge][nMale][nRisk][nAge][nMale][nRisk] = {0};

// Epislons: these parameters govern the extent to which mixing is random or assortative. Can split this up by age and risk.
int epsilons_cols = 1;
int epsilons_rows = 410; // This is time-step dependent
Eigen::MatrixXd epsilons = readCSV("epsilons_mat.csv", epsilons_cols, epsilons_rows);

// Mixing matrix for completely random mixing
double randomMat[nAge][nMale][nRisk][nAge][nMale][nRisk] = {0};

void calcMixMat(Eigen::MatrixXd &pop, int time_index) {

	// Define mixing matrix for completely assortative mixing

	// Define epsilons
	int epsilon_age = epsilons(time_index, 0);
	int epsilon_risk = epsilons(time_index, 0);

	// Risk delta is time-independent, age delta is time-dependent. Could move this out if necessary
	double delta_age = deltas(time_index, 0);

	for(int ii = 0; ii < nAge; ii++) {
		for(int jj = 0; jj < nMale; jj++) {
			for(int kk = 0; kk < nRisk; kk++) {
				for(int ii_p = 0; ii_p < nAge; ii_p++) {
					for(int jj_p = 0; jj_p < nMale; jj_p++) {
						for(int kk_p = 0; kk_p < nRisk; kk_p++) {

							// std::cout << "ii: " << ii << std::endl;
							// std::cout << "jj: " << jj << std::endl;
							// std::cout << "kk: " << kk << std::endl;
							// std::cout << "ii_p: " << ii_p << std::endl;
							// std::cout << "jj_p: " << jj_p << std::endl;
							// std::cout << "kk_p: " << kk_p << std::endl;

							if(kk == kk_p && jj != jj_p) { // Heterosexual mixing within same risk group
								assortMat[ii][jj][kk][ii_p][jj_p][kk_p] = 1.0;
							} else {
								assortMat[ii][jj][kk][ii_p][jj_p][kk_p] = 0.0;
							}

							// Multiply risk deltas by corresponding age deltas
							if(!((ii == (nAge-1) && ii_p == (nAge-1) && jj == 1 && jj_p == 0) || (ii == 0 && ii_p == 0 && jj == 0 && jj_p == 1))) {
								
								if(ii == ii_p && jj != jj_p) { // Heterosexual mixing within the same age group
	
									assortMat[ii][jj][kk][ii_p][jj_p][kk_p] *= delta_age;
	
								} else if(ii == (ii_p + 1) && jj == 0 && jj_p == 1) { // Females mixing with male partners one age group older
	
									assortMat[ii][jj][kk][ii_p][jj_p][kk_p] *= (1.0 - delta_age);
	
								} else if(ii == (ii_p - 1) && jj == 1 && jj_p == 0) { // Males mixing with female partners one age group youger
	
									assortMat[ii][jj][kk][ii_p][jj_p][kk_p] *= (1.0 - delta_age);
	
								} else {
	
									assortMat[ii][jj][kk][ii_p][jj_p][kk_p] *= 0;
	
								}
							} else {

								if(ii == (nAge-1) && ii_p == (nAge-1) && jj == 1 && jj_p == 0) { // These lines ensure probabilities sum to one for youngest males/oldest females

									assortMat[ii][jj][kk][ii_p][jj_p][kk_p] *= 1.0;

								}

								if(ii == 0 && ii_p == 0 && jj == 0 && jj_p == 1) {

									assortMat[ii][jj][kk][ii_p][jj_p][kk_p] *= 1.0;

								}
							}

							// Scale everything by 1-epsilon for both risk and age: could move this out for clarity later
							// assortMat[ii][jj][kk][ii_p][jj_p][kk_p] *= (1-epsilon_age)*(1-epsilon_risk);
						}
					}
				}
			}
		}
	}


	// CHECK IF THIS PRINTS CORRECTLY/SUMS TO 1: This is failing for males in oldest age group
	for(int ii = 0; ii < nAge; ii++) {
		for(int jj = 0; jj < nMale; jj++) {
			for(int kk = 0; kk < nRisk; kk++) {
				double counter = 0;
				for(int ii_p = 0; ii_p < nAge; ii_p++) {
					for(int jj_p = 0; jj_p < nMale; jj_p++) {
						for(int kk_p = 0; kk_p < nRisk; kk_p++) {
							counter += assortMat[ii][jj][kk][ii_p][jj_p][kk_p];
						}
					}
				}
				if(abs(counter - 1) > 1e-5) {
					std::cout << "ii: " << ii << std::endl;
					std::cout << "jj: " << jj << std::endl;
					std::cout << "kk: " << kk << std::endl;
					std::cout << "counter: " << counter << std::endl;
				}
			}
		}
	}
					

	// // Age - this delta is time-dependent
	// double delta_age = deltas(time_index, 1);

	// for(int ii = 0; ii < nAge; ii++) {
	// 	for(int jj = 0; jj < nMale; jj++) {
	// 		for(int kk = 0; kk < nRisk; kk++) {
	// 			for(ii_p = 0; ii_p < nAge; ii_p++) {
	// 				for(jj_p = 0; jj_p < nMale; jj_p++) {
	// 					for(kk_p = 0; kk_p < nRisk; kk_p++) {

	// 						if(ii == ii_p && jj != jj_p) { // Heterosexual mixing within the same age group

	// 							deltaMat_age[ii][jj][kk][ii_p][jj_p][kk_p] = delta_age;

	// 						} else if(ii == (ii_p + 1) && jj == 0 && jj_p == 1) { // Females mixing with male partners one age group older

	// 							deltaMat_age[ii][jj][kk][ii_p][jj_p][kk_p] = 1.0 - delta_age;

	// 						} else if(ii == (ii_p - 1) && jj == 1 && jj_p == 0 { // Males mixing with female partners one age group youger

	// 							deltaMat_age[ii][jj][kk][ii_p][jj_p][kk_p] = 1.0 - delta_age;

	// 						} else {

	// 							deltaMat_age[ii][jj][kk][ii_p][jj_p][kk_p] = 0;

	// 						}

	// 						if(ii == nAge && ii_p == nAge && jj == 1 && jj_p == 0) { // These lines ensure probabilities sum to one for youngest males/oldest females

	// 							deltaMat_age[ii][jj][kk][ii_p][jj_p][kk_p] = 1.0;

	// 						}

	// 						if(ii == 0 && ii_p == 0 && jj == 0 && jj_p == 1) {

	// 							deltaMat_age[ii][jj][kk][ii_p][jj_p][kk_p] = 1.0;

	// 						}

	// 					}
	// 				}
	// 			}
	// 		}
	// 	}
	// }

	// Calculate mixing matrix for completely random mixing

	// Total number of partnerships, by sex: 1x2

	// Proportion of partnerships, by age for each sex: 2x12

	// Proportion of partnerships from a given age that are in each risk group, for each sex: 2x12x3

	// Final random mixing matrix should be 2x12x3x2x12x3 matrix, with summation in each (?) dimension equal to 1 (at least certainly for the first three dimensions)



}

// void calcRisk() { // what are the inputs needed here?


	// calcMixMat

	// adjustPartnerships

	// calcLambda
// }



int main(){
    clock_t tStart;
    clock_t tEnd;
    Eigen::MatrixXd pop = readCSV("progressDisease.out", pop_cols, pop_rows);
    tStart = clock();
    calcMixMat(pop, 409);
    // calcRisk(pop, 409);
    tEnd = clock();
   	std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
    // writeCSV(pop, "calcRisk.cout");
}
