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

const int hivInd = 0;
const int ageInd = 1;
const int maleInd = 2;
const int riskInd = 3;
const int cd4Ind = 4;
const int vlInd = 5;
const int circInd = 6;
const int prepInd = 7;
const int condomInd = 8;
const int artInd = 9;
const int countInd = 10;
const int diffInd = 11;
const int timeInd = 12;

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
Eigen::MatrixXd partners_mat = readCSV("partners_mat.csv", partners_cols, partners_rows); // age, male, risk, partners
int partners_ind = 3; // Column of partners_mat containing the number of partners.

// Deltas: these parameters govern the mixing pattern for "completely assortative" mixing by age, which isn't truly "completely assortative" but is allowed to vary. The delta that gets loaded here is the proportion of partnerships that are with the same age group. 1-delta is the proportion that are with the age group one above (females) or below (males).
int deltas_cols = 1;
int deltas_rows = 410; // This is time-step dependent
Eigen::MatrixXd deltas = readCSV("deltas_smoothed.csv", deltas_cols, deltas_rows);

double assortMatAgeSex[nAge][nMale][nAge][nMale] = {0};
double assortMatRisk[nRisk][nRisk] = {0};

// Mixing matrix for completely assortative mixing
// double assortMat[nAge][nMale][nRisk][nAge][nMale][nRisk] = {0};

// Epislons: these parameters govern the extent to which mixing is random or assortative. Can split this up by age and risk but this might require refactoring the code (see how I did this in R)
int epsilons_cols = 1;
int epsilons_rows = 410; // This is time-step dependent
Eigen::MatrixXd epsilons = readCSV("epsilons_mat.csv", epsilons_cols, epsilons_rows);

// Mixing matrix for completely random mixing
double randomMat[nAge][nMale][nRisk][nAge][nMale][nRisk] = {0};

void calcMixMat(Eigen::MatrixXd &pop, int time_index) {
	 const int nPopRows = pop.rows();


	// Define epsilons. If I want different epsilons for age and risk I'll need to change this (and think carefully about whether code is still correct)
	double epsilon_age = epsilons(time_index, 0);
	double epsilon_risk = epsilons(time_index, 0);

	// Define mixing matrices for completely assortative mixing

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

	// Assortative mixing by age and sex. Only heterosexual partnerships allowed. Use deltas to define age-disparate distribution.
	double delta_age = deltas(time_index, 0);
	for(int ii = 0; ii < nAge; ii++) {
		for(int jj = 0; jj < nMale; jj++) {
			for(int ii_p = 0; ii_p < nAge; ii_p++) {
				for(int jj_p = 0; jj_p < nMale; jj_p++) {

					if(jj != jj_p) { // Only heterosexual partnerships allowed

						if(ii == ii_p) { // Partnership age groups are equal

							if((ii == (nAge-1) && ii_p == (nAge-1) && jj == 0) || (ii == 0 && ii_p == 0 && jj == 1)) { // This handles the edge groups (youngest males, oldest females) who can't have age disparate partnerships

								assortMatAgeSex[ii][jj][ii_p][jj_p] = 1; 

							} else {

								assortMatAgeSex[ii][jj][ii_p][jj_p] = delta_age;
							}

						} else if(jj == 1 && ii == (ii_p + 1)) { // Male with female partner one age group younger

							assortMatAgeSex[ii][jj][ii_p][jj_p] = (1 - delta_age);

						} else if(jj == 0 && ii == (ii_p - 1)) { // Female with male partner one age group older

							assortMatAgeSex[ii][jj][ii_p][jj_p] = (1 - delta_age);

						} else {

							assortMatAgeSex[ii][jj][ii_p][jj_p] = 0;

						}

					} else {

						assortMatAgeSex[ii][jj][ii_p][jj_p] = 0;

					}

				}
			}
		}
	}

	// Check if these assortativity matrices all sum to 1:
	std::cout << "Checking risk assortativity matrix" << std::endl;
	for(int kk = 0; kk < nRisk; kk++) {
		double counter = 0;
		for(int kk_p = 0; kk_p < nRisk; kk_p++) {
			counter += assortMatRisk[kk][kk_p];
		}
		if(abs(counter - 1) > 1e-5) {
			std::cout << "kk: " << kk << std::endl;
			std::cout << "counter: " << counter << std::endl;
		}
	}

	std::cout << "Checking age/sex assortativity matrix" << std::endl;

	for(int ii = 0; ii < nAge; ii++) {
		for(int jj = 0; jj < nMale; jj++) {
			double counter = 0;
			for(int ii_p = 0; ii_p < nAge; ii_p++) {
				for(int jj_p = 0; jj_p < nMale; jj_p++) {

					counter += assortMatAgeSex[ii][jj][ii_p][jj_p];

					// if(ii == 0 && jj == 0) {
					// 	std::cout << "ii == 0 && jj == 0" << std::endl;
					// 	std::cout << "ii_p: " << ii_p << std::endl;
					// 	std::cout << "jj_p: " << jj_p << std::endl;
					// 	std::cout << "prop: " << assortMatAgeSex[ii][jj][ii_p][jj_p] << std::endl;
					// }

				}
			}
			if(abs(counter - 1) > 1e-5) {
					std::cout << "ii: " << ii << std::endl;
					std::cout << "jj: " << jj << std::endl;
					std::cout << "counter: " << counter << std::endl;
			}
		}
	}

	// Calculate mixing matrix for completely random mixing

	// Populate partners array
	double partners[nAge][nMale][nRisk] = {0};

	for(int ii = 0; ii < partners_rows; ii++) {
	
		int iage = partners_mat(ii, 0) - 1;
		int imale = partners_mat(ii, 1);
		int irisk =  partners_mat(ii, 2) - 1;
	
		partners[iage][imale][irisk] = partners_mat(ii, partners_ind) * time_step;
	}

	// Totals
	double total_partners_sex[nMale] = {0};
	double total_partners_sex_age[nAge][nMale] = {0};
	double total_partners_sex_age_risk[nAge][nMale][nRisk] = {0};


	int ihiv, iage, imale, irisk, icd4, ivl, icirc, iprep, icondom, iart;

	for(int rowInd = 0; rowInd < nPopRows; rowInd++) {

		double sum = 0; 

        iage = pop(rowInd,ageInd) - 1; // 1 indexed fucker
        imale = pop(rowInd,maleInd);
        irisk = pop(rowInd,riskInd) - 1; // 1 indexed fucker

        sum = (pop(rowInd, countInd) * partners[iage][imale][irisk]);

        total_partners_sex[imale] += sum;
        total_partners_sex_age[iage][imale] += sum;
		total_partners_sex_age_risk[iage][imale][irisk] += sum;

	}

	// for(int ii = 0; ii < nMale; ii++) {
	// 	std::cout << "Total partners by sex: " << std::endl;
	// 	std::cout << "male: " << ii << "; " << total_partners_sex[ii] << std::endl;
	// }

	// for(int ii = 0; ii < nAge; ii++) {
	// 	for(int jj = 0; jj < nMale; jj++) {
	// 		std::cout << "Total partners by age and sex: " << std::endl;
	// 		std::cout << "age: " << ii << "; " << "male: " << jj << "; " << total_partners_sex_age[ii][jj] << std::endl;
	// 	}
	// }

	// for(int ii = 0; ii < nAge; ii++) {
	// 	for(int jj = 0; jj < nMale; jj++) {
	// 		for(int kk = 0; kk < nRisk; kk++) {
	// 		std::cout << "Total partners by age and sex and risk: " << std::endl;
	// 		std::cout << "age: " << ii << "; " << "male: " << jj << "; " << "risk: " << kk << "; " <<  total_partners_sex_age_risk[ii][jj][kk] << std::endl;
	// 		}
	// 	}
	// }



	// Proportions
	// Proportion of partners, by each for each sex (12x2)
	double prop_age_by_sex[nAge][nMale] = {0};

	for(int ii = 0; ii < nAge; ii++) {
		for(int jj = 0; jj < nMale; jj++) {

			prop_age_by_sex[ii][jj] = total_partners_sex_age[ii][jj]/total_partners_sex[jj];

		}
	}

	// for(int ii = 0; ii < nAge; ii++) {
	// 	for(int jj = 0; jj < nMale; jj++) {
	// 		std::cout << "Proportion partners by age for each sex: " << std::endl;
	// 		std::cout << "age: " << ii << "; " << "male: " << jj << "; " << prop_age_by_sex[ii][jj] << std::endl;
	// 	}
	// }


	// Proportion of partners by risk for each age/sex combination (12x2x3)
	double prop_risk_by_age_sex[nAge][nMale][nRisk] = {0};
	for(int ii = 0; ii < nAge; ii++) {
		for(int jj = 0; jj < nMale; jj++) {
			for(int kk = 0; kk < nRisk; kk++) {

				prop_risk_by_age_sex[ii][jj][kk] = total_partners_sex_age_risk[ii][jj][kk]/total_partners_sex_age[ii][jj];

			}
		}
	}


	// Calculate mixing matrix: weighted average of random mixing and assortative mixing

	// pa = prop_age * epsilon + delta_age * (1-epsilon)

	// pr_a = prop_risk_by_age * epsilon + delta_risk * (1-epsilon)

	// prop = pa * pr_a
	double pr_age; 
	double pr_risk_given_age; 

	for(int ii = 0; ii < nAge; ii++) {
		for(int jj = 0; jj < nMale; jj++) {
			for(int kk = 0; kk < nRisk; kk++) {
				for(int ii_p = 0; ii_p < nAge; ii_p++) {
					for(int jj_p = 0; jj_p < nMale; jj_p++) {
						for(int kk_p = 0; kk_p < nRisk; kk_p++) {

							pr_age = prop_age_by_sex[ii_p][jj_p] * epsilon_age + assortMatAgeSex[ii][jj][ii_p][jj_p] * (1 - epsilon_age);
							pr_risk_given_age = prop_risk_by_age_sex[ii_p][jj_p][kk_p] * epsilon_risk + assortMatRisk[kk][kk_p] * (1 - epsilon_risk);
							mixMat[ii][jj][kk][ii_p][jj_p][kk_p] = pr_age * pr_risk_given_age;

						}
					}
				}
			}
		}
	}

	// CHECK IF THIS PRINTS CORRECTLY/SUMS TO 1 (it won't once we multiply by epsilons at the end of the loop above).
	std::cout << "Checking final mixing matrix: " << std::endl;
	for(int ii = 0; ii < nAge; ii++) {
		for(int jj = 0; jj < nMale; jj++) {
			for(int kk = 0; kk < nRisk; kk++) {
				double counter = 0;
				for(int ii_p = 0; ii_p < nAge; ii_p++) {
					for(int jj_p = 0; jj_p < nMale; jj_p++) {
						for(int kk_p = 0; kk_p < nRisk; kk_p++) {
							counter += mixMat[ii][jj][kk][ii_p][jj_p][kk_p];
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

}

// void adjustPartnerships() { what are the inputs needed here?
	
// }

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
