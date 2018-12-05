#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>
#include <fstream>
#include <iomanip>
#include <cmath>

// clang++ -O3 -std=c++11 calcRisk.cpp csvUtil.cpp

// prototype -- move to header eventually
Eigen::MatrixXd readCSV(std::string filename, int cols, int rows);
void writeCSV(Eigen::MatrixXd matrix, std::string filename);

// int pop_cols = 13;
// int pop_rows = 82944;

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

double partners[nAge][nMale][nRisk] = {0}; // Array for use later

// Deltas: these parameters govern the mixing pattern for "completely assortative" mixing by age, which isn't truly "completely assortative" but is allowed to vary. The delta that gets loaded here is the proportion of partnerships that are with the same age group. 1-delta is the proportion that are with the age group one above (females) or below (males).
int deltas_cols = 1;
int deltas_rows = 410; // This is time-step dependent
Eigen::MatrixXd deltas = readCSV("deltas_smoothed.csv", deltas_cols, deltas_rows);

double assortMatAgeSex[nAge][nMale][nAge][nMale] = {0};
double assortMatRisk[nRisk][nRisk] = {0};

// Epislons: these parameters govern the extent to which mixing is random or assortative. Can split this up by age and risk but this might require refactoring the code (see how I did this in R)
int epsilons_cols = 1;
int epsilons_rows = 410; // This is time-step dependent
Eigen::MatrixXd epsilons = readCSV("epsilons_smoothed.csv", epsilons_cols, epsilons_rows);

// Mixing matrix for completely random mixing
double randomMat[nAge][nMale][nRisk][nAge][nMale][nRisk] = {0};

// Totals - these are used by more than one function
double total_partners_sex[nMale] = {0};
double total_partners_sex_age[nAge][nMale] = {0};
double total_partners_sex_age_risk[nAge][nMale][nRisk] = {0};

// Adjusted partners: array of dim mixMat containing number of adjusted partners for each partnership type
double adjustedPartnersMat[nAge][nMale][nRisk][nAge][nMale][nRisk] = {0};

// Transmission risk parameters based on number of sex acts, male, and viral load/ART status of partner
int betas_cols = 7;
int betas_rows = 72;
Eigen::MatrixXd betas_mat = readCSV("betas.csv", betas_cols, betas_rows);
int transmissionRiskInd = 6; // Column of betas_mat containing transmission risk

// Lambda matrix: final transmission risk, by age, sex, and risk. Not a mixing matrix (does not contain indices of partners). Calculated from lambda mixing matrix
double lambdaMat[nAge][nMale][nRisk] = {0};

// Risk reduction for interventions - move  these out
// int rr_cols = 2;
// int rr_rows = 3;
// Eigen::MatrixXd rr_mat = readCSV("rr_mat.csv", rr_cols, rr_rows);
// int psiInd = 1; // Column indicator for risk reduction (psi) of intervention
double psiCirc = 0.60;
double psiPrEP = 0.92;
double psiCondom = 0.78;


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

	for(int ii = 0; ii < partners_rows; ii++) {
	
		int iage = partners_mat(ii, 0) - 1;
		int imale = partners_mat(ii, 1);
		int irisk =  partners_mat(ii, 2) - 1;
	
		partners[iage][imale][irisk] = partners_mat(ii, partners_ind) * time_step;
	}


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

	// for(int ii = 0; ii < nAge; ii++) {
	// 	for(int jj = 0; jj < nMale; jj++) {
	// 		std::cout << "Proportion of partners by age and sex: " << std::endl;
	// 		std::cout << "age: " << ii << "; " << "male: " << jj << "; " << prop_age_by_sex[ii][jj] << std::endl;
	// 	}
	// }

	// for(int ii = 0; ii < nAge; ii++) {
	// 	for(int jj = 0; jj < nMale; jj++) {
	// 		for(int kk = 0; kk < nRisk; kk++) {
	// 		std::cout << "Proportion of partners by age and sex and risk: " << std::endl;
	// 		std::cout << "age: " << ii << "; " << "male: " << jj << "; " << "risk: " << kk << "; " <<  prop_risk_by_age_sex[ii][jj][kk] << std::endl;
	// 		}
	// 	}
	// }



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

							if(jj != jj_p) { // Only heterosexual mixing allowed

							pr_age = prop_age_by_sex[ii_p][jj_p] * epsilon_age + assortMatAgeSex[ii][jj][ii_p][jj_p] * (1 - epsilon_age);

							// std::cout << "ii_p: " << ii_p << std::endl;
							// std::cout << "jj_p: " << jj_p << std::endl;
							// std::cout << "kk_p: " << kk_p << std::endl;

							// std::cout << "prop_age_by_sex[ii_p][jj_p]: " << prop_age_by_sex[ii_p][jj_p] << std::endl;
							// std::cout << "epsilon_age: " << epsilon_age << std::endl;
							// std::cout << "assortMatAgeSex[ii][jj][ii_p][jj_p]: " << assortMatAgeSex[ii][jj][ii_p][jj_p] << std::endl;
							// std::cout << "(1-epsilon_age): " << (1 - epsilon_age) << std::endl;
							// std::cout << "pr_age: " << pr_age << std::endl;


							pr_risk_given_age = prop_risk_by_age_sex[ii_p][jj_p][kk_p] * epsilon_risk + assortMatRisk[kk][kk_p] * (1 - epsilon_risk);
							mixMat[ii][jj][kk][ii_p][jj_p][kk_p] = pr_age * pr_risk_given_age;

							}

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


	// Output array to match R format - maybe this can be moved into a separate function? Just for unit testing.
	std::ofstream mixMatOut ("mixing_matrix.cout");
	if(mixMatOut.is_open()){

		for(int ii = 0; ii < nAge; ii++) {
			for(int jj = 0; jj < nMale; jj++) {
				for(int kk = 0; kk < nRisk; kk++) {
					for(int ii_p = 0; ii_p < nAge; ii_p++) {
						for(int jj_p = 0; jj_p < nMale; jj_p++) {
							for(int kk_p = 0; kk_p < nRisk; kk_p++) {

								if(jj != jj_p) { // Don't print same-sex mixing, since those rows are dropped from the R output

									mixMatOut << (ii+1) << "," << jj << "," << (kk+1) << "," << (ii_p + 1) << "," << jj_p << "," << (kk_p + 1) << ","; 
									mixMatOut << std::fixed << std::setprecision(15) << mixMat[ii][jj][kk][ii_p][jj_p][kk_p] << "\n";
								}
							}
						}
					}
				}
			}
		}

	}
	


}

void adjustPartnerships() { 

	// Verify that the correct mixing matrix is getting loaded here
	std::cout << "Checking input mixing matrix: " << std::endl;
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

	// Calculate total number of partners reported by males and females
	// Multiply mixing matrix by total partners by age/sex/risk
	double sumsMat[nAge][nMale][nRisk][nAge][nMale][nRisk] = {0};

	for(int ii = 0; ii < nAge; ii++) {
		for(int jj = 0; jj < nMale; jj++) {
			for(int kk = 0; kk < nRisk; kk++) {
				for(int ii_p = 0; ii_p < nAge; ii_p++) {
					for(int jj_p = 0; jj_p < nMale; jj_p++) {
						for(int kk_p = 0; kk_p < nRisk; kk_p++) {
							sumsMat[ii][jj][kk][ii_p][jj_p][kk_p] = mixMat[ii][jj][kk][ii_p][jj_p][kk_p] * total_partners_sex_age_risk[ii][jj][kk];
						}
					}
				}
			}
		}
	}


	// For each partnership type, compare reported number of partners by males and females 
	// Divide male by female to calculate the discrepancy
	// Populate a discrepancy array (of dim mixMat) that corresponds to male reports/female reports for that partnership combination

	double male_reports;
	double female_reports;
	double disc;

	double discMat[nAge][nMale][nRisk][nAge][nMale][nRisk] = {0};

	for(int ii = 0; ii < nAge; ii++) {
		for(int jj = 0; jj < nMale; jj++) {
			for(int kk = 0; kk < nRisk; kk++) {
				for(int ii_p = 0; ii_p < nAge; ii_p++) {
					for(int jj_p = 0; jj_p < nMale; jj_p++) {
						for(int kk_p = 0; kk_p < nRisk; kk_p++) {

							if(jj != jj_p) { // Only heterosexual partnerships
								if(jj == 0) { // Females

									female_reports = sumsMat[ii][jj][kk][ii_p][jj_p][kk_p];
									male_reports = sumsMat[ii_p][jj_p][kk_p][ii][jj][kk];
									disc = male_reports/female_reports;

									discMat[ii][jj][kk][ii_p][jj_p][kk_p] = male_reports/female_reports;

								} else if(jj == 1) { // Males

									male_reports = sumsMat[ii][jj][kk][ii_p][jj_p][kk_p];
									female_reports = sumsMat[ii_p][jj_p][kk_p][ii][jj][kk];
									disc = male_reports/female_reports;

									discMat[ii][jj][kk][ii_p][jj_p][kk_p] = male_reports/female_reports;

								}
							}
						}
					}
				}
			}
		}
	}

	// std::cout << "Printing discrepancy matrix: " << std::endl;
	// for(int ii = 0; ii < nAge; ii++) {
	// 	for(int jj = 0; jj < nMale; jj++) {
	// 		for(int kk = 0; kk < nRisk; kk++) {
	// 			for(int ii_p = 0; ii_p < nAge; ii_p++) {
	// 				for(int jj_p = 0; jj_p < nMale; jj_p++) {
	// 					for(int kk_p = 0; kk_p < nRisk; kk_p++) {

	// 						if(jj != jj_p) {

	// 							std::cout << "ii: " << ii << std::endl;
	// 							std::cout << "jj: " << jj << std::endl;
	// 							std::cout << "kk: " << kk << std::endl;
	// 							std::cout << "ii_p: " << ii_p << std::endl;
	// 							std::cout << "jj_p: " << jj_p << std::endl;
	// 							std::cout << "kk_p: " << kk_p << std::endl;
	// 							std::cout << "disc: " << discMat[ii][jj][kk][ii_p][jj_p][kk_p] << std::endl;
	// 						}
		
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}
	// }

	// Calculate adjusted partnerships per year based on theta (controls the degree to which female or male reports are believed)
	// adjusted_partners = partners * discrepancy ^ (-(1 - theta))

	// Could move this into the preceding loop?
	for(int ii = 0; ii < nAge; ii++) {
		for(int jj = 0; jj < nMale; jj++) {
			for(int kk = 0; kk < nRisk; kk++) {
				for(int ii_p = 0; ii_p < nAge; ii_p++) {
					for(int jj_p = 0; jj_p < nMale; jj_p++) {
						for(int kk_p = 0; kk_p < nRisk; kk_p++) {

							if(jj != jj_p) { // Only heterosexual partnerships allowed

								if(jj == 0) { // Females

									adjustedPartnersMat[ii][jj][kk][ii_p][jj_p][kk_p] = partners[ii][jj][kk] * std::pow(discMat[ii][jj][kk][ii_p][jj_p][kk_p], theta);

								} else if (jj == 1) { // Males

									adjustedPartnersMat[ii][jj][kk][ii_p][jj_p][kk_p] = partners[ii][jj][kk] * std::pow(discMat[ii][jj][kk][ii_p][jj_p][kk_p], -(1 - theta));

								}
							}
						}
					}
				}
			}
		}
	}

	// std::cout << "Printing adjusted partners matrix: " << std::endl;
	// for(int ii = 0; ii < nAge; ii++) {
	// 	for(int jj = 0; jj < nMale; jj++) {
	// 		for(int kk = 0; kk < nRisk; kk++) {
	// 			for(int ii_p = 0; ii_p < nAge; ii_p++) {
	// 				for(int jj_p = 0; jj_p < nMale; jj_p++) {
	// 					for(int kk_p = 0; kk_p < nRisk; kk_p++) {

	// 						if(jj != jj_p) {

	// 							std::cout << "ii: " << ii << std::endl;
	// 							std::cout << "jj: " << jj << std::endl;
	// 							std::cout << "kk: " << kk << std::endl;
	// 							std::cout << "ii_p: " << ii_p << std::endl;
	// 							std::cout << "jj_p: " << jj_p << std::endl;
	// 							std::cout << "kk_p: " << kk_p << std::endl;
	// 							std::cout << "adjusted_partners: " << adjustedPartnersMat[ii][jj][kk][ii_p][jj_p][kk_p] << std::endl;
	// 						}
		
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}
	// }	

	// Output array to match R format - maybe this can be moved into a separate function? Just for unit testing.
	std::ofstream adjustedPartnersMatOut ("adjusted_partners.cout");
	if(adjustedPartnersMatOut.is_open()){

		for(int ii = 0; ii < nAge; ii++) {
			for(int jj = 0; jj < nMale; jj++) {
				for(int kk = 0; kk < nRisk; kk++) {
					for(int ii_p = 0; ii_p < nAge; ii_p++) {
						for(int jj_p = 0; jj_p < nMale; jj_p++) {
							for(int kk_p = 0; kk_p < nRisk; kk_p++) {

								if(jj != jj_p) { // Don't print same-sex mixing, since those rows are dropped from the R output

									adjustedPartnersMatOut << (ii+1) << "," << jj << "," << (kk+1) << "," << (ii_p + 1) << ","  << (kk_p + 1) << ","; 
									adjustedPartnersMatOut << std::fixed << std::setprecision(15) << adjustedPartnersMat[ii][jj][kk][ii_p][jj_p][kk_p] << "\n";
								}
							}
						}
					}
				}
			}
		}

	}
}

void calcLambda(Eigen::MatrixXd &pop) { 

	const int nPopRows = pop.rows();

	// Populate transmission risk array by partnership - should be defined outside of the function
	double betas[nMale][nRisk][nVl][nArt] = {0};

	int ihiv, iage, imale, irisk, icd4, ivl, icirc, iprep, icondom, iart, ivl_p, iart_p; // For use inside the loop
	for(int rowInd = 0; rowInd < betas_rows; rowInd++) {

		// Identify what the columns in betas_mat correspond to
		imale = betas_mat(rowInd, 0);
		irisk = betas_mat(rowInd, 1) - 1; // 1 indexed
		ivl_p = betas_mat(rowInd, 2);
		iart_p = betas_mat(rowInd, 3);

		betas[imale][irisk][ivl_p][iart_p] = betas_mat(rowInd, transmissionRiskInd);

	}

	// std::cout << "Printing betas mat: " << std::endl;
	// for(int ii = 0; ii < nMale; ii++) {
	// 	for(int jj = 0; jj < nRisk; jj++) {
	// 		for(int kk = 0; kk < nVl; kk++) {
	// 			for (int ll = 0; ll < nArt; ll ++) {

	// 				std::cout << "ii: " << ii << std::endl;
	// 				std::cout << "jj: " << jj << std::endl;
	// 				std::cout << "kk: " << kk << std::endl;
	// 				std::cout << "ll: " << ll << std::endl;
	// 				std::cout << "transmission risk: " << betas[ii][jj][kk][ll] << std::endl;


	// 			}
	// 		}
	// 	}
	// }


	// Calculate number of HIV+ people in each ART and viral load category by age, sex, and risk
	double total_hivpos_age_sex_risk_vl_art[nAge][nMale][nRisk][nVl][nArt] = {0};

	// Calculate total number of people in each age, sex, and risk category
	double total_age_sex_risk[nAge][nMale][nRisk] = {0};

	for(int rowInd = 0; rowInd < nPopRows; rowInd++) {

		ihiv = pop(rowInd, hivInd);
        iage = pop(rowInd,ageInd) - 1; // 1 indexed fucker
        imale = pop(rowInd,maleInd);
        irisk = pop(rowInd,riskInd) - 1; // 1 indexed fucker
        ivl = pop(rowInd, vlInd);
        iart = pop(rowInd, artInd);

        if(ihiv == 1) {

        	total_hivpos_age_sex_risk_vl_art[iage][imale][irisk][ivl][iart] += pop(rowInd, countInd);

        }

        total_age_sex_risk[iage][imale][irisk] += pop(rowInd, countInd);

	}

	// std::cout << "Printing total_hivpos_age_sex_risk_vl_art mat: " << std::endl;
	// for(int ii = 0; ii < nAge; ii++) {
	// 	for(int jj = 0; jj < nMale; jj++) {
	// 		for(int kk = 0; kk < nRisk; kk++) {
	// 			for (int ll = 0; ll < nVl; ll ++) {
	// 				for (int mm = 0; mm < nArt; mm ++) {

	// 					if(total_hivpos_age_sex_risk_vl_art[ii][jj][kk][ll][mm] > 1e-5) {

	// 						std::cout << "ii: " << ii << std::endl;
	// 						std::cout << "jj: " << jj << std::endl;
	// 						std::cout << "kk: " << kk << std::endl;
	// 						std::cout << "ll: " << ll << std::endl;
	// 						std::cout << "mm: " << mm << std::endl;
	// 						std::cout << "Total HIV+: " << total_hivpos_age_sex_risk_vl_art[ii][jj][kk][ll][mm] << std::endl;

	// 					}
	// 				}
	// 			}
	// 		}
	// 	}
	// }

	// std::cout << "Printing total_age_sex_risk mat: " << std::endl;
	// for(int ii = 0; ii < nAge; ii++) {
	// 	for(int jj = 0; jj < nMale; jj++) {
	// 		for(int kk = 0; kk < nRisk; kk++) {

	// 			std::cout << "ii: " << ii << std::endl;
	// 			std::cout << "jj: " << jj << std::endl;
	// 			std::cout << "kk: " << kk << std::endl;
	// 			std::cout << "Total : " << total_age_sex_risk[ii][jj][kk] << std::endl;

	// 		}
	// 	}
	// }

	// Per-partnership per year transmission risk: weighted average of transmission risk based on counts of HIV+ in each viral load category in each partnership divided by total population (HIV+ and HIV-) in each age/sex/risk category
	double ppRiskMat[nAge][nMale][nRisk][nAge][nMale][nRisk] = {0};
	double pp_risk, total_risk, adj_parts; // For use in the loop

	for(int ii = 0; ii < nAge; ii++) {
		for(int jj = 0; jj < nMale; jj++) {
			for(int kk = 0; kk < nRisk; kk++) {
				for(int ii_p = 0; ii_p < nAge; ii_p++) {
					for(int jj_p = 0; jj_p < nMale; jj_p++) {
						for(int kk_p = 0; kk_p < nRisk; kk_p++) {

							double pp_sum = 0; // Sum of per-partnership risks over ART/VL status of partner

							for(int ll_p = 0; ll_p < nVl; ll_p++) { // Looping over viral load status of partner
								for(int mm_p = 0; mm_p < nArt; mm_p++) { // Looping over ART status of partner

									if(jj != jj_p) {

						   				// Add per-partnership risk for this specific ART/VL category to running sum
						   				pp_sum += total_hivpos_age_sex_risk_vl_art[ii_p][jj_p][kk_p][ll_p][mm_p] * betas[jj][kk][ll_p][mm_p];

									}
								}
							}

							if(jj != jj_p) { // Only heterosexual parternships (this probably isn't necessary)

								// Calculated average per-partnership transmission risk for this partnership category weighted by VL and ART prevalence
								pp_risk = pp_sum / total_age_sex_risk[ii_p][jj_p][kk_p];
	
								// Calculate the number of adjusted partnerships for this partnership category
								adj_parts = mixMat[ii][jj][kk][ii_p][jj_p][kk_p] * adjustedPartnersMat[ii][jj][kk][ii_p][jj_p][kk_p];
								
								// Calculate total risk for this partnership category, which combines the per-partnership transmission risk with the number of partnerships 
								total_risk = 1 - std::pow((1 - pp_risk), adj_parts);
	
								// Store in ppRiskMat
								ppRiskMat[ii][jj][kk][ii_p][jj_p][kk_p] = total_risk;

								// if(pp_sum > 0) {
									
								// 	std::cout << "ii: " << ii << std::endl;
								// 	std::cout << "jj: " << jj << std::endl;
								// 	std::cout << "kk: " << kk << std::endl;
								// 	std::cout << "ii_p: " << ii_p << std::endl;
								// 	std::cout << "jj_p: " << jj_p << std::endl;
								// 	std::cout << "kk_p: " << kk_p << std::endl;
								// 	std::cout << "pp_sum: " << pp_sum << std::endl;
								// 	std::cout << "pp_risk: " << pp_risk << std::endl;
								// 	std::cout << "adj_parts: " << adj_parts << std::endl;
								// 	std::cout << "total_risk: " << total_risk << std::endl;

								// }


							}

						}
					}
				}
			}
		}
	}


	// Calculate the risk over all partnerships and store in lambda mat.
	for(int ii = 0; ii < nAge; ii++) {
		for(int jj = 0; jj < nMale; jj++) {
			for(int kk = 0; kk < nRisk; kk++) {

				double prod = 1;

				// lambda = 1 - prod(1 - total_risk), by age, male, and risk
				for(int ii_p = 0; ii_p < nAge; ii_p++) {
					for(int jj_p = 0; jj_p < nMale; jj_p++) {
						for(int kk_p = 0; kk_p < nRisk; kk_p++) {

							prod *= (1 - ppRiskMat[ii][jj][kk][ii_p][jj_p][kk_p]);

						}
					}
				}


				lambdaMat[ii][jj][kk] = 1 - prod;
			}
		}
	}

	// Save lambdaMat for comparison to R output
	std::ofstream lambdaMatOut ("lambda_mat.cout");
	if(lambdaMatOut.is_open()){

		for(int ii = 0; ii < nAge; ii++) {
			for(int jj = 0; jj < nMale; jj++) {
				for(int kk = 0; kk < nRisk; kk++) {

					lambdaMatOut << (ii+1) << "," << jj << "," << (kk+1) << ","; 
					lambdaMatOut << std::fixed << std::setprecision(15) << lambdaMat[ii][jj][kk] << "\n";
				
				}
			}
		}

	}

}

void transmit(Eigen::MatrixXd &pop) { 

	const int nPopRows = pop.rows(); // Any way to move this out?

	// Check to see if lambda mat is correctly loaded here:
	std::cout << "Checking input lambda matrix: " << std::endl;
	for(int ii = 0; ii < nAge; ii++) {
		for(int jj = 0; jj < nMale; jj++) {
			for(int kk = 0; kk < nRisk; kk++) {

				if(lambdaMat[ii][jj][kk] < 1e-11) {

					std::cout << "ii: " << ii << std::endl;
					std::cout << "jj: " << jj << std::endl;
					std::cout << "kk: " << kk << std::endl;
					std::cout << "lambda: " << lambdaMat[ii][jj][kk] << std::endl;

				}
			}
		}
	}


	// Create risk reduction array this would be easier to do outside the function
	double rr[nMale][nCirc][nPrep][nCondom] = {1.0};
	
	for(int ii = 0; ii < nCirc; ii++) {
		for(int jj = 0; jj < nCirc; jj++) {
			for(int kk = 0; kk < nPrep; kk++) {
				for(int ll = 0; ll < nCondom; ll++) {
	
					double psi = 1.0;
	
					if(ii == 1 && jj == 1) { // Risk reduction from circumcision - only for males
	
						psi *= (1.0 - psiCirc);
					}
	
					if(kk == 1) { // Risk reduction from PrEP
	
						psi *= (1.0 - psiPrEP);
					}
	
					if(ll == 1) { // Risk reduction from condom usage
	
						psi *= (1.0 - psiCondom);
					}

					rr[ii][jj][kk][ll] = psi;

					// if(psi < 1e-5) {
					// 	std::cout << "HERE" << std::endl;
					// }

					// std::cout << "psi: " << psi << std::endl;
	
				}
			}
		}
	}

	// Loop through pop
	// For use inside the loop
	int ihiv, iage, imale, irisk, icd4, ivl, icirc, iprep, icondom, iart;
	for (int rowInd = 0; rowInd < nPopRows; rowInd ++){

	    ihiv = pop(rowInd, hivInd);
	    iage = pop(rowInd, ageInd) - 1; // 1 indexed fucker
	    imale = pop(rowInd, maleInd);
	    irisk = pop(rowInd, riskInd) - 1; // 1 indexed fucker
	    icd4 = pop(rowInd, cd4Ind);
	    ivl = pop(rowInd, vlInd);
	    icirc = pop(rowInd, circInd);
	    iprep = pop(rowInd, prepInd);
	    icondom = pop(rowInd, condomInd);
	    iart = pop(rowInd, artInd);

	    // std::cout << "ihiv: " << ihiv << std::endl;
	    // std::cout << "iage: " << iage << std::endl;
	    // std::cout << "imale: " << imale << std::endl;
	    // std::cout << "irisk: " << irisk << std::endl;
	    // std::cout << "icd4: " << icd4 << std::endl;



	    int rowInd_hivpos;
	    double lambda;
	    double psi = 1.0;

		if(ihiv == 0) {


			// Find lambda
			lambda = lambdaMat[iage][imale][irisk];

			if(lambda < 1e-11) {

				std::cout << "lambda: " << lambda << std::endl;

			}
			

			// Find psi
			psi = rr[imale][icirc][iprep][icondom];

			if(psi < 1e-5) {

				std::cout << "psi: " << psi << std::endl;

			}
			


			// Efflux from HIV-negative population
			pop(rowInd, diffInd) -= (pop(rowInd, countInd) * lambda * psi);

			// Influx to HIV-positive population
			// Find corresponding rowIndex for HIV+ compartment
			rowInd_hivpos = rowInd + nAge * nMale * nRisk * nCD4 * nVl * nCirc * nCondom * nPrep * nArt;

			// Infections are seeded in vl = 1 and cd4 = 1 (zero-indexed). So need to find that row. This would be way easier in an array...
			int cd4_leap = nVl * nCirc * nCondom * nPrep * nArt; // Leap for each CD4 count category
			int vl_leap = nCirc * nCondom * nPrep * nArt; // Leap for each VL category

			rowInd_hivpos -= (icd4 - 1) * cd4_leap; // Adjust CD4
			rowInd_hivpos -= (ivl - 1) * vl_leap; // Adjust VL

			pop(rowInd_hivpos, diffInd) += (pop(rowInd, countInd) * lambda * psi);


		}   
	}


}



// int main(){
//     clock_t tStart;
//     clock_t tEnd;
//     Eigen::MatrixXd pop = readCSV("progressDisease.out", pop_cols, pop_rows);
//     tStart = clock();
//     calcMixMat(pop, 409);
//     tEnd = clock();
//     std::cout << "calcMixMat time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
    
//     tStart = clock();
//     adjustPartnerships();
//     tEnd = clock();
//     std::cout << "adjustPartnerships time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;

//     tStart = clock();
//     calcLambda(pop);
//     tEnd = clock();
//     std::cout << "calcLambda time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;

//     tStart = clock();
//     transmit(pop);
//     tEnd = clock();
//     std::cout << "transmit time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
//    	writeCSV(pop, "transmit.cout");
// }